#pragma once

#include <string>
#include <vector>

namespace SkillCategory {
    constexpr const char* BACKEND  = "Backend";
    constexpr const char* FRONTEND = "Frontend";
    constexpr const char* DEVOPS   = "DevOps";
    constexpr const char* QA       = "QA";
    constexpr const char* OTHER    = "Other";

    inline const std::vector<std::string>& values() {
        static const std::vector<std::string> v = {BACKEND, FRONTEND, DEVOPS, QA, OTHER};
        return v;
    }

    inline bool isValid(const std::string& skill) {
        return skill == BACKEND || skill == FRONTEND || skill == DEVOPS || skill == QA || skill == OTHER;
    }
}
