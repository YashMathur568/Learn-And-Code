#pragma once

#include <string>
#include <vector>

namespace MilestoneStatus {
    constexpr const char* NOT_STARTED  = "NOT_STARTED";
    constexpr const char* IN_PROGRESS  = "IN_PROGRESS";
    constexpr const char* DONE         = "DONE";

    inline const std::vector<std::string>& values() {
        static const std::vector<std::string> v = {NOT_STARTED, IN_PROGRESS, DONE};
        return v;
    }

    inline bool isValid(const std::string& skill) {
        return skill == NOT_STARTED || skill == IN_PROGRESS || skill == DONE;
    }
}
