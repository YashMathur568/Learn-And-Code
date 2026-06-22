#pragma once

#include <nlohmann/json.hpp>
#include <string>

class ISkillMatchService {
public:
    virtual ~ISkillMatchService() = default;

    virtual nlohmann::json findMatches(const std::string& naturalLanguageQuery) = 0;
};
