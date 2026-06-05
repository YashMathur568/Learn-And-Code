#pragma once

#include "ITimesheetRepository.hpp"
#include "../utils/DatabasePool.hpp"
#include <cppconn/resultset.h>

class MySQLTimesheetRepository : public ITimesheetRepository {
public:
    std::optional<Timesheet>    findById(int timesheetId)                                               override;
    std::optional<Timesheet>    findByEmployeeAndWeek(int employeeId, const std::string& weekStart)     override;
    std::vector<Timesheet>      findByEmployeeId(int employeeId)                                        override;
    std::vector<Timesheet>      findByManagerTeam(int managerEmployeeId, const std::string& weekStart)  override;
    int                         create(const Timesheet& timesheet)                                      override;
    std::vector<TimesheetEntry> getEntries(int timesheetId)                                             override;
    void                        addEntry(const TimesheetEntry& entry)                                   override;

private:
    static Timesheet      mapRowToTimesheet(sql::ResultSet* resultSet);
    static TimesheetEntry mapRowToEntry(sql::ResultSet* resultSet);
};
