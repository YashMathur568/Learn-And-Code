
#include "SchedulerService.hpp"
#include "AppException.hpp"
#include "ResourceStatus.hpp"

#include <mariadb/mysql.h>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

SchedulerService::SchedulerService(
    std::shared_ptr<IResourceRepository>   resourceRepository,
    std::shared_ptr<IAllocationRepository> allocationRepository,
    std::shared_ptr<IProjectRepository>    projectRepository,
    std::shared_ptr<IMilestoneRepository>  milestoneRepository,
    std::shared_ptr<ITimesheetRepository>  timesheetRepository,
    std::shared_ptr<NotificationService>   notificationService,
    int                                    intervalHours
)
    : resourceRepository_(std::move(resourceRepository))
    , allocationRepository_(std::move(allocationRepository))
    , projectRepository_(std::move(projectRepository))
    , milestoneRepository_(std::move(milestoneRepository))
    , timesheetRepository_(std::move(timesheetRepository))
    , notificationService_(std::move(notificationService))
    , intervalHours_(intervalHours) {}

SchedulerService::~SchedulerService() {
    stop();
}

void SchedulerService::start() {
    running_.store(true);
    thread_ = std::thread(&SchedulerService::loop, this);
}

void SchedulerService::stop() {
    running_.store(false);
    if (thread_.joinable()) {
        thread_.join();
    }
}

void SchedulerService::loop() {
    mysql_thread_init();

    runOnce();
    const auto interval = std::chrono::hours(intervalHours_);
    auto nextRun = std::chrono::steady_clock::now() + interval;

    while (running_.load()) {
        std::this_thread::sleep_for(std::chrono::seconds(30));
        if (std::chrono::steady_clock::now() >= nextRun) {
            runOnce();
            nextRun = std::chrono::steady_clock::now() + interval;
        }
    }

    mysql_thread_end();
}

void SchedulerService::runOnce() {
    try {
        recomputeResourceStatuses();
        recomputeProjectHealth();
        markMissedTimesheets();
        if (notificationService_) {
            notificationService_->processTimesheetReminders();
        }
    } catch (const std::exception& exception) {
        std::cerr << "[Scheduler] Error: " << exception.what() << std::endl;
    }
}

void SchedulerService::recomputeResourceStatuses() {
    const auto resources = resourceRepository_->findAllActive();
    for (const auto& resource : resources) {
        const std::vector<Allocation> active = allocationRepository_->findActiveByUserId(resource.userId);
        const std::string newStatus = active.empty() ? ResourceStatus::BENCH : ResourceStatus::ALLOCATED;
        if (resource.status != newStatus) {
            resourceRepository_->setStatus(resource.userId, newStatus);
        }
    }
}

void SchedulerService::recomputeProjectHealth() {
    const auto projects = projectRepository_->findAll();
    const std::string today = todayString();

    for (const auto& project : projects) {
        if (project.status == "ON_HOLD") {
            continue;
        }

        const auto milestones = milestoneRepository_->findByProjectId(project.projectId);

        bool hasOverdue = false;
        bool hasAtRisk  = false;

        for (const auto& milestone : milestones) {
            if (milestone.status == "DONE") {
                continue;
            }
            if (milestone.dueDate < today) {
                hasOverdue = true;
            } else {
                const auto allocations = allocationRepository_->findActiveByProjectId(project.projectId);
                if (allocations.empty()) {
                    hasAtRisk = true;
                }
            }
        }

        std::string newHealth = "ON_TRACK";
        if (hasOverdue) {
            newHealth = "AT_RISK";
        } else if (hasAtRisk) {
            newHealth = "ATTENTION";
        }

        if (project.health != newHealth) {
            projectRepository_->updateHealth(project.projectId, newHealth);
            if (newHealth == "AT_RISK" && notificationService_) {
                try {
                    notificationService_->sendAtRiskAlert(project.projectId);
                } catch (const std::exception& exception) {
                    std::cerr << "[Scheduler] AT_RISK alert failed: " << exception.what() << std::endl;
                }
            }
        }
    }
}

void SchedulerService::markMissedTimesheets() {
    const auto resources   = resourceRepository_->findAllActive();
    const auto pastMondays = previousMondays(4);

    for (const auto& resource : resources) {
        const auto allAllocations = allocationRepository_->findByUserId(resource.userId);
        if (allAllocations.empty()) continue;

        for (const auto& weekStart : pastMondays) {

            bool wasAllocated = false;
            for (const auto& allocation : allAllocations) {
                if (allocation.fromDate <= weekStart && allocation.toDate >= weekStart) {
                    wasAllocated = true;
                    break;
                }
            }
            if (!wasAllocated) continue;

            const auto existing = timesheetRepository_->findByUserAndWeek(resource.userId, weekStart);
            if (!existing.has_value()) {
                timesheetRepository_->createMissed(resource.userId, weekStart);
            }
        }
    }
}

std::string SchedulerService::todayString() {
    const std::time_t now = std::time(nullptr);
    std::tm nowTimeStructure = {};
    localtime_r(&now, &nowTimeStructure);
    std::ostringstream out;
    out << std::put_time(&nowTimeStructure, "%Y-%m-%d");
    return out.str();
}

std::string SchedulerService::lastMondayString() {
    const std::time_t now = std::time(nullptr);
    std::tm nowTimeStructure = {};
    localtime_r(&now, &nowTimeStructure);
    const int daysToSubtract = (nowTimeStructure.tm_wday == 0) ? 6 : (nowTimeStructure.tm_wday - 1);
    const std::time_t lastMonday = now - (daysToSubtract * 86400);
    std::tm mondayTimeStructure = {};
    localtime_r(&lastMonday, &mondayTimeStructure);
    std::ostringstream out;
    out << std::put_time(&mondayTimeStructure, "%Y-%m-%d");
    return out.str();
}

std::vector<std::string> SchedulerService::previousMondays(int count) {
    std::vector<std::string> result;
    const std::time_t now = std::time(nullptr);
    std::tm nowTimeStructure = {};
    localtime_r(&now, &nowTimeStructure);
    const int daysToLastMonday = (nowTimeStructure.tm_wday == 0) ? 6 : (nowTimeStructure.tm_wday - 1);

    for (int week = 1; week <= count; ++week) {
        const std::time_t mondayTime = now - ((daysToLastMonday + (week - 1) * 7) * 86400);
        std::tm mondayTimeStructure = {};
        localtime_r(&mondayTime, &mondayTimeStructure);
        std::ostringstream out;
        out << std::put_time(&mondayTimeStructure, "%Y-%m-%d");
        result.push_back(out.str());
    }
    return result;
}
