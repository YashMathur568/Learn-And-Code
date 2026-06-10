#pragma once

#include "ISchedulerService.hpp"
#include "../repositories/IEmployeeRepository.hpp"
#include "../repositories/IAllocationRepository.hpp"
#include "../repositories/IProjectRepository.hpp"
#include "../repositories/IMilestoneRepository.hpp"
#include "../repositories/ITimesheetRepository.hpp"

#include <atomic>
#include <memory>
#include <thread>

class SchedulerService : public ISchedulerService {
public:
    SchedulerService(
        std::shared_ptr<IEmployeeRepository>   employeeRepository,
        std::shared_ptr<IAllocationRepository> allocationRepository,
        std::shared_ptr<IProjectRepository>    projectRepository,
        std::shared_ptr<IMilestoneRepository>  milestoneRepository,
        std::shared_ptr<ITimesheetRepository>  timesheetRepository,
        int                                    intervalHours
    );

    ~SchedulerService();

    void start()   override;
    void stop()    override;
    void runOnce() override;

private:
    void loop();
    void recomputeEmployeeStatuses();
    void recomputeProjectHealth();
    void markMissedTimesheets();

    static std::string todayString();
    static std::string lastMondayString();
    static std::vector<std::string> previousMondays(int count);

    std::shared_ptr<IEmployeeRepository>   employeeRepository_;
    std::shared_ptr<IAllocationRepository> allocationRepository_;
    std::shared_ptr<IProjectRepository>    projectRepository_;
    std::shared_ptr<IMilestoneRepository>  milestoneRepository_;
    std::shared_ptr<ITimesheetRepository>  timesheetRepository_;

    int              intervalHours_;
    std::thread      thread_;
    std::atomic_bool running_{false};
};
