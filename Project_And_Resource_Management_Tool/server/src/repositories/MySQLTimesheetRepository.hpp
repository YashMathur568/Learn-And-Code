#pragma once

#include "ITimesheetRepository.hpp"
#include "../utils/DatabasePool.hpp"
#include <cppconn/resultset.h>

class MySQLTimesheetRepository : public ITimesheetRepository {
public:
    std::optional<Timesheet>    findById(int timesheetId)                                          override;
    std::optional<Timesheet>    findByUserAndWeek(int userId, const std::string& weekStart)        override;
    std::vector<Timesheet>      findByUserId(int userId)                                           override;
    std::vector<Timesheet>      findByManagerTeam(int managerUserId, const std::string& weekStart) override;
    std::vector<Timesheet>      findMissedForWeek(const std::string& weekStart)                    override;
    int                         create(const Timesheet& timesheet)                                 override;
    void                        createMissed(int userId, const std::string& weekStart)             override;
    void                        updateStatus(int timesheetId, const std::string& status)           override;
    std::vector<TimesheetEntry> getEntries(int timesheetId)                                        override;
    void                        addEntry(const TimesheetEntry& entry)                              override;

private:
    static Timesheet      mapRowToTimesheet(sql::ResultSet* resultSet);
    static TimesheetEntry mapRowToEntry(sql::ResultSet* resultSet);
};
