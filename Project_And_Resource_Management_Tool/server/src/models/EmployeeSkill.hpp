#pragma once

#include <string>

struct EmployeeSkill {
    int         skillId{0};
    int         employeeId{0};
    std::string skillName;
    std::string category;
    std::string proficiency;
};
