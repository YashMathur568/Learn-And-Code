#pragma once

#include "../ai/ILLMAdapter.hpp"
#include "../utils/DatabasePool.hpp"

#include <memory>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

class SkillMatchService {
public:
    explicit SkillMatchService(std::shared_ptr<ILLMAdapter> llmAdapter);

    nlohmann::json findMatches(const std::string& naturalLanguageQuery);

private:
    static std::string buildSqlPrompt(const std::string& query);
    static std::string extractSql(const std::string& llmResponse);
    static std::vector<nlohmann::json> executeQuery(const std::string& sql);
    std::string buildRankingPrompt(const std::string& query,
                                   const std::vector<nlohmann::json>& rows);
    static nlohmann::json parseRankedResult(const std::string& llmResponse);

    std::shared_ptr<ILLMAdapter> llmAdapter_;
};
