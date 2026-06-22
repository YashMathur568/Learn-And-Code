#pragma once

#include <string>
#include <vector>

namespace Proficiency {
    constexpr const char* BEGINNER     = "Beginner";
    constexpr const char* INTERMEDIATE = "Intermediate";
    constexpr const char* ADVANCED     = "Advanced";

    inline const std::vector<std::string>& values() {
        static const std::vector<std::string> v = {BEGINNER, INTERMEDIATE, ADVANCED};
        return v;
    }

    inline bool isValid(const std::string& skill) {
        return skill == BEGINNER || skill == INTERMEDIATE || skill == ADVANCED;
    }


    inline int weight(const std::string& skill) {
        if (skill == ADVANCED)     return 3;
        if (skill == INTERMEDIATE) return 2;
        return 1;
    }
}
