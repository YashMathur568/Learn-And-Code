#pragma once

#include <string>

struct TimesheetEntry {
    int         entryId{0};
    int         timesheetId{0};
    int         projectId{0};
    std::string projectName;
    int         hours{0};
    std::string activityTags;
};
