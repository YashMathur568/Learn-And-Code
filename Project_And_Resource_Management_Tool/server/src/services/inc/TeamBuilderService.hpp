#pragma once

#include "ILLMAdapter.hpp"
#include "DatabasePool.hpp"

#include <memory>
#include <string>
#include <vector>
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

    struct Skill {
        std::string name;
        std::string proficiency;
        std::string category;
    };

    struct Candidate {
        int         userId{0};
        std::string fullName;
        std::string department;
        std::string designation;
        std::string status;
        std::string allocatedUntil;
        std::vector<Skill> allSkills;  // full skill profile, not just matched skills
    };

    struct GapInfo {
        bool        hasSkillInCompany{false};
        std::string earliestAvailable;
    };

    std::vector<RoleRequirement>  parseRoles(const std::string& description);
    std::vector<Candidate>        queryCandidatesFullSkills(const RoleRequirement& role);
    GapInfo                       buildGapInfo(const RoleRequirement& role);
    std::string                   buildTeamAssemblyPrompt(
                                      const std::string& description,
                                      const std::vector<RoleRequirement>& roles,
                                      const std::vector<std::vector<Candidate>>& candidatesByRole,
                                      const std::vector<GapInfo>& gapInfoByRole);
    nlohmann::json                parseTeamResult(const std::string& raw);

    static std::string buildParsePrompt(const std::string& description);
    static std::string extractJsonArray(const std::string& raw);
    static std::string skillWhereClause(const std::vector<std::string>& skills);
    static std::string escapeSql(const std::string& input);

    std::shared_ptr<ILLMAdapter> llmAdapter_;
};
