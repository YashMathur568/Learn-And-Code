#pragma once

#include <string>
#include <vector>

namespace ProjectStatus {
    constexpr const char* PLANNED  = "PLANNED";
    constexpr const char* ACTIVE   = "ACTIVE";
    constexpr const char* ON_HOLD  = "ON_HOLD";

    inline const std::vector<std::string>& values() {
        static const std::vector<std::string> v = {PLANNED, ACTIVE, ON_HOLD};
        return v;
    }

    inline bool isValid(const std::string& skill) {
        return skill == PLANNED || skill == ACTIVE || skill == ON_HOLD;
    }
}
