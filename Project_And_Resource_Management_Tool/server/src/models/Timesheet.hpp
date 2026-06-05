#pragma once

#include <string>

struct Timesheet {
    int         timesheetId{0};
    int         employeeId{0};
    std::string weekStart;
    std::string status;
    std::string submittedAt;
};
