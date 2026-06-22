#include "SkillMatchService.hpp"
#include "SqlValidator.hpp"
#include "AppException.hpp"

#include <cppconn/statement.h>
#include <cppconn/resultset.h>
#include <cppconn/resultset_metadata.h>
#include <regex>
#include <sstream>

static constexpr const char* SCHEMA_CONTEXT = R"(
Database schema for allocation Project Resource Manager:

TABLE users (user_id INT PK, full_name VARCHAR, email VARCHAR, username VARCHAR, role_id INT FK->roles, is_active TINYINT)
TABLE roles (role_id INT PK, role_name VARCHAR)  -- role_name values: 'ADMIN', 'MANAGER', 'RESOURCE'
TABLE user_profile (user_id INT PK FK->users, manager_id INT NULL FK->users, department VARCHAR, designation VARCHAR)
TABLE resource_status (user_id INT PK FK->users, status ENUM('BENCH','ALLOCATED'))
TABLE user_skills (skill_id INT PK, user_id INT FK->users, skill_name VARCHAR, category ENUM('Backend','Frontend','DevOps','QA','Other'), proficiency ENUM('Beginner','Intermediate','Advanced'))
TABLE allocations (allocation_id INT PK, user_id INT FK->users, project_id INT FK->projects, utilisation INT, from_date DATE, to_date DATE, is_active TINYINT)
TABLE projects (project_id INT PK, name VARCHAR, description TEXT, start_date DATE, end_date DATE, status ENUM('PLANNED','ACTIVE','ON_HOLD'), manager_id INT FK->users, health ENUM('ON_TRACK','ATTENTION','AT_RISK'))

Rules:
- Only active resources: user.is_active = 1 AND ro.role_name = 'RESOURCE'
- Only output SELECT SQL
- Always join: users user, roles ro ON ro.role_id = user.role_id, user_profile rp ON rp.user_id = user.user_id, resource_status rs ON rs.user_id = user.user_id, user_skills skill ON skill.user_id = user.user_id
- Include: user.user_id, user.full_name, rp.department, rp.designation, rs.status, skill.skill_name, skill.category, skill.proficiency
- LIMIT 20
- NEVER filter by skill.proficiency in the WHERE clause, even if the query mentions 'senior', 'experienced', 'advanced', 'junior', etc. Proficiency is used only for ranking in a separate step, not for filtering.
)";

static constexpr const char* SQL_FEW_SHOT = R"(
Examples:
Q: find React developers
SQL: SELECT user.user_id, user.full_name, rp.department, rp.designation, rs.status, skill.skill_name, skill.category, skill.proficiency FROM users user JOIN roles ro ON ro.role_id = user.role_id JOIN user_profile rp ON rp.user_id = user.user_id JOIN resource_status rs ON rs.user_id = user.user_id JOIN user_skills skill ON skill.user_id = user.user_id WHERE user.is_active = 1 AND ro.role_name = 'RESOURCE' AND LOWER(skill.skill_name) LIKE '%react%' LIMIT 20;

Q: who knows Python with Advanced proficiency
SQL: SELECT user.user_id, user.full_name, rp.department, rp.designation, rs.status, skill.skill_name, skill.category, skill.proficiency FROM users user JOIN roles ro ON ro.role_id = user.role_id JOIN user_profile rp ON rp.user_id = user.user_id JOIN resource_status rs ON rs.user_id = user.user_id JOIN user_skills skill ON skill.user_id = user.user_id WHERE user.is_active = 1 AND ro.role_name = 'RESOURCE' AND LOWER(skill.skill_name) = 'python' LIMIT 20;

Q: bench resources with DevOps skills
SQL: SELECT user.user_id, user.full_name, rp.department, rp.designation, rs.status, skill.skill_name, skill.category, skill.proficiency FROM users user JOIN roles ro ON ro.role_id = user.role_id JOIN user_profile rp ON rp.user_id = user.user_id JOIN resource_status rs ON rs.user_id = user.user_id JOIN user_skills skill ON skill.user_id = user.user_id WHERE user.is_active = 1 AND ro.role_name = 'RESOURCE' AND rs.status = 'BENCH' AND skill.category = 'DevOps' LIMIT 20;

Q: someone with C++ experience
SQL: SELECT user.user_id, user.full_name, rp.department, rp.designation, rs.status, skill.skill_name, skill.category, skill.proficiency FROM users user JOIN roles ro ON ro.role_id = user.role_id JOIN user_profile rp ON rp.user_id = user.user_id JOIN resource_status rs ON rs.user_id = user.user_id JOIN user_skills skill ON skill.user_id = user.user_id WHERE user.is_active = 1 AND ro.role_name = 'RESOURCE' AND LOWER(skill.skill_name) LIKE '%c++%' LIMIT 20;
)";

SkillMatchService::SkillMatchService(std::shared_ptr<ILLMAdapter> llmAdapter)
    : llmAdapter_(std::move(llmAdapter)) {}

nlohmann::json SkillMatchService::findMatches(const std::string& naturalLanguageQuery) {
    const std::string sqlPrompt = buildSqlPrompt(naturalLanguageQuery);
    const std::string sqlRaw    = llmAdapter_->generate(sqlPrompt);
    const std::string sql       = extractSql(sqlRaw);

    SqlValidator::validate(sql);

    const auto rows = executeQuery(sql);

    if (rows.empty()) {
        return nlohmann::json::array();
    }

    const std::string rankPrompt = buildRankingPrompt(naturalLanguageQuery, rows);
    const std::string rankRaw    = llmAdapter_->generate(rankPrompt);
    return parseRankedResult(rankRaw);
}

std::string SkillMatchService::buildSqlPrompt(const std::string& query) {
    std::ostringstream prompt;
    prompt << SCHEMA_CONTEXT << "\n" << SQL_FEW_SHOT << "\n"
           << "Now generate ONLY the SQL SELECT statement for this query. "
           << "Output only the raw SQL, no explanation, no markdown, no backticks.\n"
           << "Q: " << query << "\nSQL:";
    return prompt.str();
}

std::string SkillMatchService::extractSql(const std::string& llmResponse) {
    std::string sql = llmResponse;

    const std::regex codeBlock(R"(```(?:sql)?\skill*([\skill\S]*?)```)");
    std::smatch match;
    if (std::regex_search(sql, match, codeBlock)) {
        sql = match[1].str();
    }

    const size_t selectPos = sql.find("SELECT");
    const size_t selectPosLower = sql.find("select");
    const size_t pos = (selectPos != std::string::npos) ? selectPos :
                       (selectPosLower != std::string::npos) ? selectPosLower :
                       std::string::npos;

    if (pos != std::string::npos) {
        sql = sql.substr(pos);
    }

    const size_t semicolon = sql.find(';');
    if (semicolon != std::string::npos) {
        sql = sql.substr(0, semicolon + 1);
    }

    while (!sql.empty() && (sql.back() == '\n' || sql.back() == '\r' || sql.back() == ' ')) {
        sql.pop_back();
    }
    return sql;
}

std::vector<nlohmann::json> SkillMatchService::executeQuery(const std::string& sql) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::Statement> statement(connection->createStatement());
        std::unique_ptr<sql::ResultSet> resultSet(statement->executeQuery(sql));
        sql::ResultSetMetaData* metaData = resultSet->getMetaData();
        const unsigned int columnCount = metaData->getColumnCount();

        std::vector<nlohmann::json> rows;
        while (resultSet->next()) {
            nlohmann::json row;
            for (unsigned int columnIndex = 1; columnIndex <= columnCount; ++columnIndex) {
                const std::string columnName(metaData->getColumnName(columnIndex).c_str());
                if (resultSet->isNull(columnIndex)) {
                    row[columnName] = nullptr;
                } else {
                    row[columnName] = resultSet->getString(columnIndex);
                }
            }
            rows.push_back(row);
        }
        return rows;
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("SQL execution error: ") + sqlException.what());
    }
}

std::string SkillMatchService::buildRankingPrompt(
    const std::string& query,
    const std::vector<nlohmann::json>& rows
) {
    std::ostringstream prompt;
    prompt << "You are a resource allocation assistant.\n"
           << "The manager asked: \"" << query << "\"\n\n"
           << "These resources were found in the database:\n"
           << nlohmann::json(rows).dump(2) << "\n\n"
           << "Rank these resources by relevance to the query.\n"
           << "Scoring guidelines:\n"
           << "- If the query mentions 'senior', 'experienced', 'advanced': give higher scores to Advanced proficiency, lower to Intermediate, lowest to Beginner.\n"
           << "- If the query mentions 'junior', 'entry-level', 'beginner': reverse the scoring.\n"
           << "- BENCH status is preferred over ALLOCATED.\n"
           << "- Group rows by user_id and list all their skills.\n"
           << "Return ONLY a valid JSON array. Each element must have:\n"
           << "  resourceId (integer), fullName (string), department (string), "
           << "designation (string), status (string), "
           << "skills (array of {skillName, proficiency}), "
           << "relevanceScore (1-10 integer), reason (one sentence string)\n"
           << "Output only the JSON array, no explanation, no markdown.";
    return prompt.str();
}

nlohmann::json SkillMatchService::parseRankedResult(const std::string& llmResponse) {
    std::string cleaned = llmResponse;

    const std::regex codeBlock(R"(```(?:json)?\skill*([\skill\S]*?)```)");
    std::smatch match;
    if (std::regex_search(cleaned, match, codeBlock)) {
        cleaned = match[1].str();
    }

    const size_t arrayStart = cleaned.find('[');
    const size_t arrayEnd   = cleaned.rfind(']');
    if (arrayStart != std::string::npos && arrayEnd != std::string::npos) {
        cleaned = cleaned.substr(arrayStart, arrayEnd - arrayStart + 1);
    }

    try {
        return nlohmann::json::parse(cleaned);
    } catch (const std::exception&) {
        throw AppException("Failed to parse LLM ranking response as JSON.");
    }
}
