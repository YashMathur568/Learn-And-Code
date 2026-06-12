#include "SkillMatchService.hpp"
#include "../ai/SqlValidator.hpp"
#include "../utils/AppException.hpp"

#include <cppconn/statement.h>
#include <cppconn/resultset.h>
#include <cppconn/resultset_metadata.h>
#include <regex>
#include <sstream>

static constexpr const char* SCHEMA_CONTEXT = R"(
Database schema for a Project Resource Manager:

TABLE users (user_id INT PK, full_name VARCHAR, email VARCHAR, username VARCHAR, role_id INT FK->roles, is_active TINYINT)
TABLE roles (role_id INT PK, role_name VARCHAR)  -- role_name values: 'ADMIN', 'MANAGER', 'RESOURCE'
TABLE resource_profile (user_id INT PK FK->users, manager_id INT NULL FK->users, department VARCHAR, designation VARCHAR)
TABLE resource_status (user_id INT PK FK->users, status ENUM('BENCH','ALLOCATED'))
TABLE user_skills (skill_id INT PK, user_id INT FK->users, skill_name VARCHAR, category ENUM('Backend','Frontend','DevOps','QA','Other'), proficiency ENUM('Beginner','Intermediate','Advanced'))
TABLE allocations (allocation_id INT PK, user_id INT FK->users, project_id INT FK->projects, utilisation INT, from_date DATE, to_date DATE, is_active TINYINT)
TABLE projects (project_id INT PK, name VARCHAR, description TEXT, start_date DATE, end_date DATE, status ENUM('PLANNED','ACTIVE','ON_HOLD'), manager_id INT FK->users, health ENUM('ON_TRACK','ATTENTION','AT_RISK'))

Rules:
- Only active resources: u.is_active = 1 AND ro.role_name = 'RESOURCE'
- Only output SELECT SQL
- Always join: users u, roles ro ON ro.role_id = u.role_id, resource_profile rp ON rp.user_id = u.user_id, resource_status rs ON rs.user_id = u.user_id, user_skills s ON s.user_id = u.user_id
- Include: u.user_id, u.full_name, rp.department, rp.designation, rs.status, s.skill_name, s.category, s.proficiency
- LIMIT 20
)";

static constexpr const char* SQL_FEW_SHOT = R"(
Examples:
Q: find React developers
SQL: SELECT u.user_id, u.full_name, rp.department, rp.designation, rs.status, s.skill_name, s.category, s.proficiency FROM users u JOIN roles ro ON ro.role_id = u.role_id JOIN resource_profile rp ON rp.user_id = u.user_id JOIN resource_status rs ON rs.user_id = u.user_id JOIN user_skills s ON s.user_id = u.user_id WHERE u.is_active = 1 AND ro.role_name = 'RESOURCE' AND LOWER(s.skill_name) LIKE '%react%' LIMIT 20;

Q: who knows Python with Advanced proficiency
SQL: SELECT u.user_id, u.full_name, rp.department, rp.designation, rs.status, s.skill_name, s.category, s.proficiency FROM users u JOIN roles ro ON ro.role_id = u.role_id JOIN resource_profile rp ON rp.user_id = u.user_id JOIN resource_status rs ON rs.user_id = u.user_id JOIN user_skills s ON s.user_id = u.user_id WHERE u.is_active = 1 AND ro.role_name = 'RESOURCE' AND LOWER(s.skill_name) = 'python' AND s.proficiency = 'Advanced' LIMIT 20;

Q: bench employees with DevOps skills
SQL: SELECT u.user_id, u.full_name, rp.department, rp.designation, rs.status, s.skill_name, s.category, s.proficiency FROM users u JOIN roles ro ON ro.role_id = u.role_id JOIN resource_profile rp ON rp.user_id = u.user_id JOIN resource_status rs ON rs.user_id = u.user_id JOIN user_skills s ON s.user_id = u.user_id WHERE u.is_active = 1 AND ro.role_name = 'RESOURCE' AND rs.status = 'BENCH' AND s.category = 'DevOps' LIMIT 20;

Q: someone with C++ experience
SQL: SELECT u.user_id, u.full_name, rp.department, rp.designation, rs.status, s.skill_name, s.category, s.proficiency FROM users u JOIN roles ro ON ro.role_id = u.role_id JOIN resource_profile rp ON rp.user_id = u.user_id JOIN resource_status rs ON rs.user_id = u.user_id JOIN user_skills s ON s.user_id = u.user_id WHERE u.is_active = 1 AND ro.role_name = 'RESOURCE' AND LOWER(s.skill_name) LIKE '%c++%' LIMIT 20;
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

    const std::regex codeBlock(R"(```(?:sql)?\s*([\s\S]*?)```)");
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
        std::unique_ptr<sql::Statement> stmt(connection->createStatement());
        std::unique_ptr<sql::ResultSet> rs(stmt->executeQuery(sql));
        sql::ResultSetMetaData* meta = rs->getMetaData();
        const unsigned int colCount = meta->getColumnCount();

        std::vector<nlohmann::json> rows;
        while (rs->next()) {
            nlohmann::json row;
            for (unsigned int col = 1; col <= colCount; ++col) {
                const std::string colName(meta->getColumnName(col).c_str());
                if (rs->isNull(col)) {
                    row[colName] = nullptr;
                } else {
                    row[colName] = rs->getString(col);
                }
            }
            rows.push_back(row);
        }
        return rows;
    } catch (const sql::SQLException& ex) {
        throw AppException(std::string("SQL execution error: ") + ex.what());
    }
}

std::string SkillMatchService::buildRankingPrompt(
    const std::string& query,
    const std::vector<nlohmann::json>& rows
) {
    std::ostringstream prompt;
    prompt << "You are a resource allocation assistant.\n"
           << "The manager asked: \"" << query << "\"\n\n"
           << "These employees were found in the database:\n"
           << nlohmann::json(rows).dump(2) << "\n\n"
           << "Rank these employees by relevance to the query. "
           << "Group rows by employee_id and list all their skills. "
           << "Return ONLY a valid JSON array. Each element must have:\n"
           << "  employeeId (integer), fullName (string), department (string), "
           << "designation (string), status (string), "
           << "skills (array of {skillName, proficiency}), "
           << "relevanceScore (1-10 integer), reason (one sentence string)\n"
           << "Output only the JSON array, no explanation, no markdown.";
    return prompt.str();
}

nlohmann::json SkillMatchService::parseRankedResult(const std::string& llmResponse) {
    std::string cleaned = llmResponse;

    const std::regex codeBlock(R"(```(?:json)?\s*([\s\S]*?)```)");
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
