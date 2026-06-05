#pragma once

#include "../dto/AllocationDtos.hpp"
#include "../models/Timesheet.hpp"
#include "../models/TimesheetEntry.hpp"
#include "../repositories/ITimesheetRepository.hpp"
#include "../repositories/IAllocationRepository.hpp"
#include "../repositories/IEmployeeRepository.hpp"

#include <memory>
#include <string>
#include <vector>

class TimesheetService {
public:
    TimesheetService(
        std::shared_ptr<ITimesheetRepository>  timesheetRepository,
        std::shared_ptr<IAllocationRepository> allocationRepository,
        std::shared_ptr<IEmployeeRepository>   employeeRepository
    );

    TimesheetWithEntries              submitTimesheet(int employeeId, const SubmitTimesheetRequest& request);
    std::vector<TimesheetWithEntries> getByEmployeeId(int employeeId);
    std::vector<TimesheetWithEntries> getTeamTimesheets(int managerEmployeeId, const std::string& weekStart);

private:
    std::shared_ptr<ITimesheetRepository>  timesheetRepository;
    std::shared_ptr<IAllocationRepository> allocationRepository;
    std::shared_ptr<IEmployeeRepository>   employeeRepository;

    static bool isMondayDate(const std::string& dateStr);
    static bool isWeekInFuture(const std::string& weekStart);
};
