#pragma once

#include <string>

struct Timesheet {
    int         timesheetId{0};
    int         userId{0};
    std::string weekStart;
    std::string status;
    std::string submittedAt;
};
