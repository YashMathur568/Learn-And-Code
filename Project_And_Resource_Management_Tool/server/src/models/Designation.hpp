#pragma once

#include <string>
#include <vector>

namespace Designation {
    constexpr const char* INTERN         = "Intern";
    constexpr const char* JUNIOR         = "Junior";
    constexpr const char* MID_LEVEL      = "Mid-Level";
    constexpr const char* SENIOR         = "Senior";
    constexpr const char* LEAD           = "Lead";
    constexpr const char* PRINCIPAL      = "Principal";
    constexpr const char* MANAGER        = "Manager";
    constexpr const char* SENIOR_MANAGER = "Senior Manager";
    constexpr const char* DIRECTOR       = "Director";
    constexpr const char* VP             = "VP";
    constexpr const char* C_LEVEL        = "C-Level";
    constexpr const char* ADMINISTRATOR  = "Administrator";

    inline const std::vector<std::string>& values() {
        static const std::vector<std::string> designations = {
            INTERN, JUNIOR, MID_LEVEL, SENIOR, LEAD,
            PRINCIPAL, MANAGER, SENIOR_MANAGER, DIRECTOR, VP, C_LEVEL, ADMINISTRATOR
        };
        return designations;
    }

    inline bool isValid(const std::string& designationName) {
        for (const auto& desig : values()) if (desig == designationName) return true;
        return false;
    }
}
