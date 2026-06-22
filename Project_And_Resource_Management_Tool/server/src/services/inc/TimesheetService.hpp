#pragma once

#include "ITimesheetService.hpp"
#include "ITimesheetRepository.hpp"
#include "IAllocationRepository.hpp"
#include "IResourceRepository.hpp"

#include <memory>
#include <string>
#include <vector>

class TimesheetService : public ITimesheetService {
public:
    TimesheetService(
        std::shared_ptr<ITimesheetRepository>  timesheetRepository,
        std::shared_ptr<IAllocationRepository> allocationRepository,
        std::shared_ptr<IResourceRepository>   resourceRepository
    );

    TimesheetWithEntries              submitTimesheet(int userId,
                                                      const SubmitTimesheetRequest& request) override;
    std::vector<TimesheetWithEntries> getByUserId(int userId)                               override;
    std::vector<TimesheetWithEntries> getTeamTimesheets(int managerUserId,
                                                         const std::string& weekStart)      override;

private:
    std::shared_ptr<ITimesheetRepository>  timesheetRepository;
    std::shared_ptr<IAllocationRepository> allocationRepository;
    std::shared_ptr<IResourceRepository>   resourceRepository;

    static bool        isMondayDate(const std::string& dateStr);
    static bool        isWeekInFuture(const std::string& weekStart);
    static std::string getCurrentWeekMonday();
};
