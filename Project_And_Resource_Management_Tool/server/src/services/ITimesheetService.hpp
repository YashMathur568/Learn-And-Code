#pragma once

#include "../dto/AllocationDtos.hpp"

#include <string>
#include <vector>

class ITimesheetService {
public:
    virtual ~ITimesheetService() = default;

    virtual TimesheetWithEntries              submitTimesheet(int userId,
                                                              const SubmitTimesheetRequest& request) = 0;
    virtual std::vector<TimesheetWithEntries> getByUserId(int userId)                               = 0;
    virtual std::vector<TimesheetWithEntries> getTeamTimesheets(int managerUserId,
                                                                 const std::string& weekStart)       = 0;
};
