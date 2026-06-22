#pragma once

#include <string>
#include <vector>

namespace ProjectHealth {
    constexpr const char* ON_TRACK  = "ON_TRACK";
    constexpr const char* ATTENTION = "ATTENTION";
    constexpr const char* AT_RISK   = "AT_RISK";

    inline const std::vector<std::string>& values() {
        static const std::vector<std::string> v = {ON_TRACK, ATTENTION, AT_RISK};
        return v;
    }

    inline bool isValid(const std::string& skill) {
        return skill == ON_TRACK || skill == ATTENTION || skill == AT_RISK;
    }
}
