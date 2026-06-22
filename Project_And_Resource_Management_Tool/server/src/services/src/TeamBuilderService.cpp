#include "TeamBuilderService.hpp"
#include "AppException.hpp"

#include <cppconn/statement.h>
#include <cppconn/resultset.h>
#include <cppconn/resultset_metadata.h>
#include <algorithm>
#include <map>
#include <regex>
#include <sstream>





TeamBuilderService::TeamBuilderService(std::shared_ptr<ILLMAdapter> llmAdapter)
    : llmAdapter_(std::move(llmAdapter)) {}





std::string TeamBuilderService::escapeSql(const std::string& input) {
    std::string out;
    out.reserve(input.size());
    for (char ch : input) {
        if (ch == '\'') out += "''";
        else            out += ch;
    }
    return out;
}

std::string TeamBuilderService::skillWhereClause(const std::vector<std::string>& skills) {
    std::ostringstream clause;
    for (size_t skillIndex = 0; skillIndex < skills.size(); ++skillIndex) {
        if (skillIndex > 0) clause << " OR ";
        clause << "LOWER(skill.skill_name) LIKE '%" << escapeSql(skills[skillIndex]) << "%'";
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





std::string TeamBuilderService::buildParsePrompt(const std::string& description) {
    std::ostringstream promptStream;
    promptStream << "You are a resource planning assistant.\n"
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
    return promptStream.str();
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
    for (const auto& roleRequirementJson : rolesJson) {
        RoleRequirement roleRequirement;
        roleRequirement.roleName       = roleRequirementJson.value("roleName", "Unnamed Role");
        roleRequirement.minProficiency = roleRequirementJson.value("minProficiency", "Beginner");
        for (const auto& skillNameJson : roleRequirementJson.value("skills", nlohmann::json::array()))
            if (skillNameJson.is_string()) roleRequirement.skills.push_back(skillNameJson.get<std::string>());
        roles.push_back(std::move(roleRequirement));
    }
    return roles;
}





std::vector<TeamBuilderService::Candidate>
TeamBuilderService::queryCandidatesFullSkills(const RoleRequirement& role) {
    if (role.skills.empty()) return {};

    // The inner subquery finds user_ids who match any required skill.
    // The outer query then fetches ALL skills for those users — not just the
    // matched ones — so the AI assembly step sees each candidate's full profile.
    const std::string innerWhere = skillWhereClause(role.skills);
    const std::string sql =
        "SELECT user.user_id, user.full_name, rp.department, rp.designation, rs.status, "
        "skill.skill_name, skill.proficiency, skill.category, "
        "(SELECT DATE_FORMAT(MAX(a2.`to_date`), '%Y-%m-%d') "
        " FROM allocations a2 "
        " WHERE a2.user_id = user.user_id AND a2.is_active = 1 AND a2.`to_date` >= CURDATE()) "
        " AS allocated_until "
        "FROM users user "
        "JOIN roles ro ON ro.role_id = user.role_id "
        "JOIN user_profile rp ON rp.user_id = user.user_id "
        "JOIN resource_status rs ON rs.user_id = user.user_id "
        "JOIN user_skills skill ON skill.user_id = user.user_id "
        "WHERE user.is_active = 1 AND ro.role_name = 'RESOURCE' "
        "  AND user.user_id IN ("
        "    SELECT DISTINCT user_id FROM user_skills "
        "    WHERE (" + innerWhere + ")"
        "  ) "
        "ORDER BY user.user_id, skill.skill_name";

    std::map<int, Candidate> byId;
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::Statement> statement(connection->createStatement());
        std::unique_ptr<sql::ResultSet> resultSet(statement->executeQuery(sql));

        while (resultSet->next()) {
            const int userId = resultSet->getInt("user_id");
            if (byId.find(userId) == byId.end()) {
                Candidate candidate;
                candidate.userId      = userId;
                candidate.fullName    = resultSet->getString("full_name").c_str();
                candidate.department  = resultSet->getString("department").c_str();
                candidate.designation = resultSet->getString("designation").c_str();
                candidate.status      = resultSet->getString("status").c_str();
                if (!resultSet->isNull("allocated_until"))
                    candidate.allocatedUntil = resultSet->getString("allocated_until").c_str();
                byId[userId] = std::move(candidate);
            }

            Skill skill;
            skill.name        = resultSet->getString("skill_name").c_str();
            skill.proficiency = resultSet->getString("proficiency").c_str();
            skill.category    = resultSet->getString("category").c_str();
            byId[userId].allSkills.push_back(std::move(skill));
        }
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in team builder: ") + sqlException.what());
    }

    std::vector<Candidate> candidates;
    candidates.reserve(byId.size());
    for (auto& [id, c] : byId)
        candidates.push_back(std::move(c));

    // Sort: BENCH first, then by total skill count (richer profile = more useful for AI)
    std::sort(candidates.begin(), candidates.end(), [](const Candidate& a, const Candidate& b) {
        if (a.status != b.status) return a.status < b.status;
        return a.allSkills.size() > b.allSkills.size();
    });

    // Cap at 10 per role to keep the AI prompt size manageable
    if (candidates.size() > 10)
        candidates.resize(10);

    return candidates;
}





TeamBuilderService::GapInfo
TeamBuilderService::buildGapInfo(const RoleRequirement& role) {
    GapInfo info;
    if (role.skills.empty()) return info;

    const std::string sql =
        "SELECT user.user_id, rs.status, "
        "(SELECT DATE_FORMAT(MAX(a2.`to_date`), '%Y-%m-%d') "
        " FROM allocations a2 "
        " WHERE a2.user_id = user.user_id AND a2.is_active = 1 AND a2.`to_date` >= CURDATE()) "
        " AS free_after "
        "FROM users user "
        "JOIN roles ro ON ro.role_id = user.role_id "
        "JOIN resource_status rs ON rs.user_id = user.user_id "
        "JOIN user_skills skill ON skill.user_id = user.user_id "
        "WHERE user.is_active = 1 AND ro.role_name = 'RESOURCE' "
        "AND (" + skillWhereClause(role.skills) + ") "
        "GROUP BY user.user_id, rs.status";

    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::Statement> statement(connection->createStatement());
        std::unique_ptr<sql::ResultSet> resultSet(statement->executeQuery(sql));
        while (resultSet->next()) {
            info.hasSkillInCompany = true;
            if (!resultSet->isNull("free_after")) {
                const std::string date = resultSet->getString("free_after").c_str();
                if (info.earliestAvailable.empty() || date < info.earliestAvailable)
                    info.earliestAvailable = date;
            }
        }
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in gap analysis: ") + sqlException.what());
    }
    return info;
}

std::string TeamBuilderService::buildTeamAssemblyPrompt(
    const std::string& description,
    const std::vector<RoleRequirement>& roles,
    const std::vector<std::vector<Candidate>>& candidatesByRole,
    const std::vector<GapInfo>& gapInfoByRole
) {
    // Build the roles summary for the prompt
    nlohmann::json rolesJson = nlohmann::json::array();
    for (const auto& role : roles) {
        rolesJson.push_back({
            {"roleName",       role.roleName},
            {"requiredSkills", role.skills},
            {"minProficiency", role.minProficiency}
        });
    }

    // Build each candidate's full profile grouped by role
    nlohmann::json candidatesJson = nlohmann::json::object();
    for (size_t i = 0; i < roles.size(); ++i) {
        nlohmann::json roleArr = nlohmann::json::array();
        for (const auto& c : candidatesByRole[i]) {
            nlohmann::json skillsArr = nlohmann::json::array();
            for (const auto& s : c.allSkills)
                skillsArr.push_back({{"name", s.name}, {"proficiency", s.proficiency}, {"category", s.category}});

            nlohmann::json cj;
            cj["userId"]         = c.userId;
            cj["fullName"]       = c.fullName;
            cj["department"]     = c.department;
            cj["designation"]    = c.designation;
            cj["status"]         = c.status;
            cj["allocatedUntil"] = c.allocatedUntil.empty()
                                   ? nlohmann::json(nullptr)
                                   : nlohmann::json(c.allocatedUntil);
            cj["skills"]         = skillsArr;
            roleArr.push_back(std::move(cj));
        }
        candidatesJson[roles[i].roleName] = roleArr;
    }

    // Build gap notes for roles where no candidates were found in DB
    std::ostringstream gapNotes;
    bool hasGaps = false;
    for (size_t i = 0; i < roles.size(); ++i) {
        if (candidatesByRole[i].empty()) {
            hasGaps = true;
            const auto& gap = gapInfoByRole[i];
            gapNotes << "  - " << roles[i].roleName << ": ";
            if (!gap.hasSkillInCompany)
                gapNotes << "No one in the company has this skill. Consider hiring or training.";
            else if (gap.earliestAvailable.empty())
                gapNotes << "People with this skill exist but all are allocated with no recorded end date.";
            else
                gapNotes << "All people with this skill are allocated. Earliest free: " << gap.earliestAvailable << ".";
            gapNotes << "\n";
        }
    }

    std::ostringstream prompt;
    prompt << "You are a resource allocation assistant.\n"
           << "The manager wants to build a team for: \"" << description << "\"\n\n"
           << "Required roles:\n" << rolesJson.dump(2) << "\n\n"
           << "Available candidates per role (each person's full skill profile from the database):\n"
           << candidatesJson.dump(2) << "\n\n";

    if (hasGaps)
        prompt << "Gap information for roles with no candidates found:\n" << gapNotes.str() << "\n";

    prompt << "Build the optimal team following these rules:\n"
           << "- Assign each person to AT MOST ONE role (no double-booking across roles)\n"
           << "- Prefer BENCH status over ALLOCATED\n"
           << "- Prefer candidates whose proficiency meets or exceeds minProficiency\n"
           << "- A candidate missing some required skills but having others is still valid — assign with a lower score\n"
           << "- If a role has candidates but all are taken by other roles, set filled=false\n"
           << "- If a role has no candidates at all, use the gap information above\n\n"
           << "Return ONLY a valid JSON array. Each element must have:\n"
           << "  roleName (string), filled (boolean)\n"
           << "  If filled=true: userId (integer), fullName (string), department (string),\n"
           << "    designation (string), status (string), allocatedUntil (string or null),\n"
           << "    matchedSkills (array of skill name strings relevant to this role),\n"
           << "    relevanceScore (1-10 integer), reason (one sentence why this person fits)\n"
           << "  If filled=false: gapReason (string explaining why the role could not be filled)\n\n"
           << "Output only the raw JSON array, no markdown, no explanation.";

    return prompt.str();
}

nlohmann::json TeamBuilderService::parseTeamResult(const std::string& raw) {
    std::string cleaned = raw;
    const std::regex codeBlock(R"(```(?:json)?\s*([\s\S]*?)```)");
    std::smatch match;
    if (std::regex_search(cleaned, match, codeBlock))
        cleaned = match[1].str();

    const size_t start = cleaned.find('[');
    const size_t end   = cleaned.rfind(']');
    if (start == std::string::npos || end == std::string::npos)
        throw AppException("AI returned an invalid team suggestion. Please rephrase and try again.");

    cleaned = cleaned.substr(start, end - start + 1);
    try {
        return nlohmann::json::parse(cleaned);
    } catch (const std::exception&) {
        throw AppException("Failed to parse AI team assembly response.");
    }
}

nlohmann::json TeamBuilderService::buildTeam(const std::string& description) {
    // Step 1: AI parses the natural language description into structured roles
    const auto roles = parseRoles(description);

    // Step 2: For each role, query the DB for candidates with their full skill profiles
    std::vector<std::vector<Candidate>> candidatesByRole;
    std::vector<GapInfo>                gapInfoByRole;
    for (const auto& role : roles) {
        auto candidates = queryCandidatesFullSkills(role);
        GapInfo gapInfo;
        if (candidates.empty())
            gapInfo = buildGapInfo(role);
        candidatesByRole.push_back(std::move(candidates));
        gapInfoByRole.push_back(std::move(gapInfo));
    }

    // Step 3: AI assembles the optimal team, reasons about trade-offs, avoids double-booking
    const std::string assemblyPrompt = buildTeamAssemblyPrompt(description, roles, candidatesByRole, gapInfoByRole);
    const std::string assemblyRaw    = llmAdapter_->generate(assemblyPrompt);
    auto result = parseTeamResult(assemblyRaw);

    // Step 4: Post-process — override gapReason for roles with zero DB candidates with
    // our computed GapInfo (AI tends to write vague messages; we have the precise data).
    if (result.is_array()) {
        for (auto& roleResult : result) {
            if (roleResult.value("filled", false)) continue;

            const std::string aiRoleName = roleResult.value("roleName", "");
            std::string lowerAi = aiRoleName;
            std::transform(lowerAi.begin(), lowerAi.end(), lowerAi.begin(), ::tolower);

            for (size_t i = 0; i < roles.size(); ++i) {
                if (!candidatesByRole[i].empty()) continue; // only override when we had no candidates

                std::string lowerOrig = roles[i].roleName;
                std::transform(lowerOrig.begin(), lowerOrig.end(), lowerOrig.begin(), ::tolower);

                // Match role names by substring (AI may rephrase slightly)
                if (lowerAi.find(lowerOrig) == std::string::npos &&
                    lowerOrig.find(lowerAi) == std::string::npos) continue;

                const auto& gap = gapInfoByRole[i];
                std::string reason;
                if (!gap.hasSkillInCompany) {
                    reason = "No one in the company has this skill. Consider hiring or training.";
                } else if (gap.earliestAvailable.empty()) {
                    reason = "People with this skill exist but are all currently allocated with no recorded end date.";
                } else {
                    reason = "People with this skill are all currently allocated. Earliest available: " + gap.earliestAvailable + ".";
                }
                roleResult["gapReason"] = reason;
                break;
            }
        }
    }

    return result;
}
