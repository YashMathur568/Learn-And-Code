#pragma once

#include "../dto/AllocationDtos.hpp"

#include <string>
#include <vector>

class ITimesheetService {
public:
    virtual ~ITimesheetService() = default;

    virtual TimesheetWithEntries              submitTimesheet(int employeeId,
                                                              const SubmitTimesheetRequest& request) = 0;
    virtual std::vector<TimesheetWithEntries> getByEmployeeId(int employeeId)                        = 0;
    virtual std::vector<TimesheetWithEntries> getTeamTimesheets(int managerEmployeeId,
                                                                const std::string& weekStart)        = 0;
};
