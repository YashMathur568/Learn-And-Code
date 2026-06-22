#pragma once

#include <string>
#include <vector>

namespace ResourceStatus {
    constexpr const char* BENCH     = "BENCH";
    constexpr const char* ALLOCATED = "ALLOCATED";

    inline const std::vector<std::string>& values() {
        static const std::vector<std::string> v = {BENCH, ALLOCATED};
        return v;
    }

    inline bool isValid(const std::string& skill) {
        return skill == BENCH || skill == ALLOCATED;
    }
}
