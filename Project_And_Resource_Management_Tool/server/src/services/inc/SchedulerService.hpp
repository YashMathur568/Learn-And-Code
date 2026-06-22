#pragma once

#include "ISchedulerService.hpp"
#include "NotificationService.hpp"
#include "IResourceRepository.hpp"
#include "IAllocationRepository.hpp"
#include "IProjectRepository.hpp"
#include "IMilestoneRepository.hpp"
#include "ITimesheetRepository.hpp"

#include <atomic>
#include <memory>
#include <thread>

class SchedulerService : public ISchedulerService {
public:
    SchedulerService(
        std::shared_ptr<IResourceRepository>   resourceRepository,
        std::shared_ptr<IAllocationRepository> allocationRepository,
        std::shared_ptr<IProjectRepository>    projectRepository,
        std::shared_ptr<IMilestoneRepository>  milestoneRepository,
        std::shared_ptr<ITimesheetRepository>  timesheetRepository,
        std::shared_ptr<NotificationService>   notificationService,
        int                                    intervalHours
    );

    ~SchedulerService();

    void start()   override;
    void stop()    override;
    void runOnce() override;

private:
    void loop();
    void recomputeResourceStatuses();
    void recomputeProjectHealth();
    void markMissedTimesheets();

    static std::string todayString();
    static std::string lastMondayString();
    static std::vector<std::string> previousMondays(int count);

    std::shared_ptr<IResourceRepository>   resourceRepository_;
    std::shared_ptr<IAllocationRepository> allocationRepository_;
    std::shared_ptr<IProjectRepository>    projectRepository_;
    std::shared_ptr<IMilestoneRepository>  milestoneRepository_;
    std::shared_ptr<ITimesheetRepository>  timesheetRepository_;
    std::shared_ptr<NotificationService>   notificationService_;

    int              intervalHours_;
    std::thread      thread_;
    std::atomic_bool running_{false};
};
