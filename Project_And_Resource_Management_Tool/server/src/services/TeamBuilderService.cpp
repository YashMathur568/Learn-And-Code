#include "TeamBuilderService.hpp"
#include "../utils/AppException.hpp"

#include <cppconn/statement.h>
#include <cppconn/resultset.h>
#include <cppconn/resultset_metadata.h>
#include <algorithm>
#include <map>
#include <regex>
#include <sstream>

// ─────────────────────────────────────────────────────────
// Constructor
// ─────────────────────────────────────────────────────────

TeamBuilderService::TeamBuilderService(std::shared_ptr<ILLMAdapter> llmAdapter)
    : llmAdapter_(std::move(llmAdapter)) {}

// ─────────────────────────────────────────────────────────
// Helpers
// ─────────────────────────────────────────────────────────

std::string TeamBuilderService::escapeSql(const std::string& input) {
    std::string out;
    out.reserve(input.size());
    for (char ch : input) {
        if (ch == '\'') out += "''";
        else            out += ch;
    }
    return out;
}

int TeamBuilderService::proficiencyWeight(const std::string& p) {
    if (p == "Advanced")     return 3;
    if (p == "Intermediate") return 2;
    return 1;
}

// Builds: LOWER(s.skill_name) LIKE '%java%' OR LOWER(s.skill_name) LIKE '%spring%'
std::string TeamBuilderService::skillWhereClause(const std::vector<std::string>& skills) {
    std::ostringstream clause;
    for (size_t idx = 0; idx < skills.size(); ++idx) {
        if (idx > 0) clause << " OR ";
        clause << "LOWER(s.skill_name) LIKE '%" << escapeSql(skills[idx]) << "%'";
    }
    return clause.str();
}

std::string TeamBuilderService::extractJsonArray(const std::string& raw) {
    std::string cleaned = raw;
    const std::regex codeBlock(R"(```(?:json)?\s*([\s\S]*?)```)");
    std::smatch match;
    if (std::regex_search(cleaned, match, codeBlock))
        cleaned = match[1].str();

    const size_t start = cleaned.find('[');
    const size_t end   = cleaned.rfind(']');
    if (start == std::string::npos || end == std::string::npos)
        throw AppException("AI could not parse the team description. Please rephrase and try again.");

    return cleaned.substr(start, end - start + 1);
}

// ─────────────────────────────────────────────────────────
// LLM: parse natural-language description → structured roles
// ─────────────────────────────────────────────────────────

std::string TeamBuilderService::buildParsePrompt(const std::string& description) {
    std::ostringstream p;
    p << "You are a resource planning assistant.\n"
      << "Parse the following team requirement into a JSON array of role objects.\n"
      << "Each role must have exactly these fields:\n"
      << "  roleName       (string)\n"
      << "  skills         (array of skill name strings, e.g. [\"Java\",\"Spring Boot\"])\n"
      << "  minProficiency (one of: \"Beginner\", \"Intermediate\", \"Advanced\")\n\n"
      << "Rules:\n"
      << "- If 'senior' or 'advanced' is mentioned → minProficiency = \"Advanced\"\n"
      << "- If 'junior' or 'entry-level' is mentioned → minProficiency = \"Beginner\"\n"
      << "- Otherwise → minProficiency = \"Intermediate\"\n"
      << "- Output ONLY the raw JSON array, no markdown, no explanation.\n\n"
      << "Example:\n"
      << "Input: I need a Senior Java Dev and a QA Tester with Selenium\n"
      << "Output: [{\"roleName\":\"Senior Java Developer\",\"skills\":[\"Java\",\"Spring Boot\"],"
         "\"minProficiency\":\"Advanced\"},{\"roleName\":\"QA Tester\",\"skills\":[\"Selenium\","
         "\"Testing\"],\"minProficiency\":\"Beginner\"}]\n\n"
      << "Input: " << description << "\n"
      << "Output:";
    return p.str();
}

std::vector<TeamBuilderService::RoleRequirement>
TeamBuilderService::parseRoles(const std::string& description) {
    const std::string raw     = llmAdapter_->generate(buildParsePrompt(description));
    const std::string cleaned = extractJsonArray(raw);

    nlohmann::json rolesJson;
    try {
        rolesJson = nlohmann::json::parse(cleaned);
    } catch (...) {
        throw AppException("AI returned invalid role JSON. Please rephrase and try again.");
    }
    if (!rolesJson.is_array() || rolesJson.empty())
        throw AppException("No roles could be parsed from your description.");

    std::vector<RoleRequirement> roles;
    for (const auto& r : rolesJson) {
        RoleRequirement req;
        req.roleName       = r.value("roleName", "Unnamed Role");
        req.minProficiency = r.value("minProficiency", "Beginner");
        for (const auto& s : r.value("skills", nlohmann::json::array()))
            if (s.is_string()) req.skills.push_back(s.get<std::string>());
        roles.push_back(std::move(req));
    }
    return roles;
}

// ─────────────────────────────────────────────────────────
// DB: find candidates for a role (all employees with matching skills)
// ─────────────────────────────────────────────────────────

std::vector<TeamBuilderService::Candidate>
TeamBuilderService::queryCandidates(const RoleRequirement& role) {
    if (role.skills.empty()) return {};

    // One row per (user, skill) — subquery gives their latest active allocation end date
    const std::string sql =
        "SELECT u.user_id, u.full_name, rp.department, rp.designation, rs.status, "
        "s.skill_name, s.proficiency, "
        "(SELECT DATE_FORMAT(MAX(a2.`to_date`), '%Y-%m-%d') "
        " FROM allocations a2 "
        " WHERE a2.user_id = u.user_id AND a2.is_active = 1 AND a2.`to_date` >= CURDATE()) "
        " AS allocated_until "
        "FROM users u "
        "JOIN roles ro ON ro.role_id = u.role_id "
        "JOIN resource_profile rp ON rp.user_id = u.user_id "
        "JOIN resource_status rs ON rs.user_id = u.user_id "
        "JOIN user_skills s ON s.user_id = u.user_id "
        "WHERE u.is_active = 1 AND ro.role_name = 'RESOURCE' "
        "AND (" + skillWhereClause(role.skills) + ")";

    // Group results by userId in C++
    std::map<int, Candidate> byId;
    try {
        auto conn = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::Statement> stmt(conn->createStatement());
        std::unique_ptr<sql::ResultSet> rs(stmt->executeQuery(sql));

        while (rs->next()) {
            const int uid = rs->getInt("user_id");
            if (byId.find(uid) == byId.end()) {
                Candidate c;
                c.userId       = uid;
                c.fullName     = rs->getString("full_name").c_str();
                c.department   = rs->getString("department").c_str();
                c.designation  = rs->getString("designation").c_str();
                c.status       = rs->getString("status").c_str();
                if (!rs->isNull("allocated_until"))
                    c.allocatedUntil = rs->getString("allocated_until").c_str();
                // Bench bonus added once
                if (c.status == "BENCH") c.score += 10;
                byId[uid] = std::move(c);
            }
            // Accumulate skill score
            const std::string proficiency = rs->getString("proficiency").c_str();
            const std::string skillName   = rs->getString("skill_name").c_str();
            byId[uid].matchedSkills.push_back(skillName);
            byId[uid].score += proficiencyWeight(proficiency);
        }
    } catch (const sql::SQLException& ex) {
        throw AppException(std::string("DB error in team builder: ") + ex.what());
    }

    std::vector<Candidate> candidates;
    candidates.reserve(byId.size());
    for (auto& [id, c] : byId)
        candidates.push_back(std::move(c));

    // Sort: bench first, then highest score
    std::sort(candidates.begin(), candidates.end(), [](const Candidate& a, const Candidate& b) {
        if (a.status != b.status) return a.status < b.status; // "BENCH" < "ALLOCATED"
        return a.score > b.score;
    });
    return candidates;
}

// ─────────────────────────────────────────────────────────
// Gap analysis: why couldn't this role be filled?
// ─────────────────────────────────────────────────────────

std::string TeamBuilderService::buildGapReason(const RoleRequirement& role) {
    if (role.skills.empty())
        return "No skills specified for this role.";

    // Find any employee (bench OR allocated) who has ANY required skill
    const std::string sql =
        "SELECT u.user_id, rs.status, "
        "(SELECT DATE_FORMAT(MAX(a2.`to_date`), '%d-%m-%Y') "
        " FROM allocations a2 "
        " WHERE a2.user_id = u.user_id AND a2.is_active = 1 AND a2.`to_date` >= CURDATE()) "
        " AS free_after "
        "FROM users u "
        "JOIN roles ro ON ro.role_id = u.role_id "
        "JOIN resource_status rs ON rs.user_id = u.user_id "
        "JOIN user_skills s ON s.user_id = u.user_id "
        "WHERE u.is_active = 1 AND ro.role_name = 'RESOURCE' "
        "AND (" + skillWhereClause(role.skills) + ") "
        "GROUP BY u.user_id, rs.status";

    struct GapRow { std::string status; std::string freeAfter; };
    std::vector<GapRow> found;
    try {
        auto conn = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::Statement> stmt(conn->createStatement());
        std::unique_ptr<sql::ResultSet> rs(stmt->executeQuery(sql));
        while (rs->next()) {
            GapRow gr;
            gr.status    = rs->getString("status").c_str();
            if (!rs->isNull("free_after"))
                gr.freeAfter = rs->getString("free_after").c_str();
            found.push_back(gr);
        }
    } catch (const sql::SQLException& ex) {
        throw AppException(std::string("DB error in gap analysis: ") + ex.what());
    }

    if (found.empty()) {
        // Build a comma-separated list of the primary skills
        std::string skillList;
        for (size_t idx = 0; idx < role.skills.size() && idx < 3; ++idx) {
            if (idx > 0) skillList += ", ";
            skillList += role.skills[idx];
        }
        return "Skill '" + skillList + "' not found in company — consider hiring or training.";
    }

    // Someone has the skill but is allocated — find earliest free date
    std::string earliest;
    for (const auto& gr : found) {
        if (!gr.freeAfter.empty()) {
            if (earliest.empty() || gr.freeAfter < earliest)
                earliest = gr.freeAfter;
        }
    }
    if (earliest.empty())
        return "Matching employees exist but are currently unavailable (no end date recorded).";

    return "All employees with this skill are currently allocated. "
           "Earliest availability: " + earliest + ".";
}

// ─────────────────────────────────────────────────────────
// Main entry point
// ─────────────────────────────────────────────────────────

nlohmann::json TeamBuilderService::buildTeam(const std::string& description) {
    const auto roles = parseRoles(description);

    std::set<int> usedIds; // tracks already-assigned employees
    nlohmann::json result  = nlohmann::json::array();

    for (const auto& role : roles) {
        nlohmann::json roleResult;
        roleResult["roleName"] = role.roleName;

        const auto candidates = queryCandidates(role);

        // Pick best candidate not yet assigned
        const Candidate* best = nullptr;
        for (const auto& c : candidates) {
            if (usedIds.count(c.userId) == 0) { best = &c; break; }
        }

        if (best) {
            usedIds.insert(best->userId);
            roleResult["filled"]         = true;
            roleResult["userId"]         = best->userId;
            roleResult["fullName"]       = best->fullName;
            roleResult["department"]     = best->department;
            roleResult["designation"]    = best->designation;
            roleResult["status"]         = best->status;
            roleResult["allocatedUntil"] = best->allocatedUntil;
            roleResult["matchedSkills"]  = best->matchedSkills;
        } else if (!candidates.empty()) {
            // Candidates exist but all were already assigned to earlier roles
            roleResult["filled"]    = false;
            roleResult["gapReason"] = "All " + std::to_string(candidates.size()) +
                                      " employee(s) matching this role have already been "
                                      "assigned to other roles in this team.";
        } else {
            // No one in company has the required skills
            roleResult["filled"]    = false;
            roleResult["gapReason"] = buildGapReason(role);
        }
        result.push_back(roleResult);
    }
    return result;
}
