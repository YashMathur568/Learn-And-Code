#pragma once

#include "../ai/ILLMAdapter.hpp"
#include "../utils/DatabasePool.hpp"

#include <memory>
#include <string>
#include <vector>
#include <set>
#include <nlohmann/json.hpp>

class TeamBuilderService {
public:
    explicit TeamBuilderService(std::shared_ptr<ILLMAdapter> llmAdapter);

    nlohmann::json buildTeam(const std::string& description);

private:
    struct RoleRequirement {
        std::string roleName;
        std::vector<std::string> skills;
        std::string minProficiency;
    };

    struct Candidate {
        int         userId{0};
        std::string fullName;
        std::string department;
        std::string designation;
        std::string status;         // "BENCH" or "ALLOCATED"
        std::string allocatedUntil; // ISO date, empty if bench
        std::vector<std::string> matchedSkills;
        int         score{0};
    };

    std::vector<RoleRequirement> parseRoles(const std::string& description);
    std::vector<Candidate>       queryCandidates(const RoleRequirement& role);
    std::string                  buildGapReason(const RoleRequirement& role);

    static std::string buildParsePrompt(const std::string& description);
    static std::string extractJsonArray(const std::string& raw);
    static int         proficiencyWeight(const std::string& p);
    static std::string skillWhereClause(const std::vector<std::string>& skills);
    static std::string escapeSql(const std::string& s);

    std::shared_ptr<ILLMAdapter> llmAdapter_;
};
