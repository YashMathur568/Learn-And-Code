#pragma once

#include <string>

struct ResourceSkill {
    int         skillId{0};
    int         userId{0};
    std::string skillName;
    std::string category;
    std::string proficiency;
};
