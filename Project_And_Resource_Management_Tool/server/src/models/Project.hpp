#pragma once

#include <string>

struct Project {
    int         projectId{0};
    std::string name;
    std::string description;
    std::string startDate;
    std::string endDate;
    std::string status;
    int         managerId{0};
    std::string health;
    int         totalStoryPoints{0};
    int         completedStoryPoints{0};
};
