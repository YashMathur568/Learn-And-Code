#pragma once

#include "../models/Timesheet.hpp"
#include "../models/TimesheetEntry.hpp"
#include <optional>
#include <string>
#include <vector>

class ITimesheetRepository {
public:
    virtual ~ITimesheetRepository() = default;

    virtual std::optional<Timesheet>    findById(int timesheetId)                                           = 0;
    virtual std::optional<Timesheet>    findByUserAndWeek(int userId, const std::string& weekStart)         = 0;
    virtual std::vector<Timesheet>      findByUserId(int userId)                                            = 0;
    virtual std::vector<Timesheet>      findByManagerTeam(int managerUserId, const std::string& weekStart)  = 0;
    virtual int                         create(const Timesheet& timesheet)                                  = 0;
    virtual void                        createMissed(int userId, const std::string& weekStart)              = 0;
    virtual std::vector<TimesheetEntry> getEntries(int timesheetId)                                         = 0;
    virtual void                        addEntry(const TimesheetEntry& entry)                               = 0;
};
