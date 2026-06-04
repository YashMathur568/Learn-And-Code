#pragma once

#include <string>

struct Milestone {
    int         milestoneId{0};
    int         projectId{0};
    std::string title;
    std::string dueDate;
    std::string status;
};
