
#include "SchedulerService.hpp"
#include "../utils/AppException.hpp"

#include <mariadb/mysql.h>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>

SchedulerService::SchedulerService(
    std::shared_ptr<IEmployeeRepository>   employeeRepository,
    std::shared_ptr<IAllocationRepository> allocationRepository,
    std::shared_ptr<IProjectRepository>    projectRepository,
    std::shared_ptr<IMilestoneRepository>  milestoneRepository,
    std::shared_ptr<ITimesheetRepository>  timesheetRepository,
    int                                    intervalHours
)
    : employeeRepository_(std::move(employeeRepository))
    , allocationRepository_(std::move(allocationRepository))
    , projectRepository_(std::move(projectRepository))
    , milestoneRepository_(std::move(milestoneRepository))
    , timesheetRepository_(std::move(timesheetRepository))
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
        recomputeEmployeeStatuses();
        recomputeProjectHealth();
        markMissedTimesheets();
    } catch (const std::exception& ex) {
        std::cerr << "[Scheduler] Error: " << ex.what() << std::endl;
    }
}

void SchedulerService::recomputeEmployeeStatuses() {
    const auto employees = employeeRepository_->findAllActive();
    for (const auto& employee : employees) {
        const std::vector<Allocation> active = allocationRepository_->findActiveByEmployeeId(employee.employeeId);
        const std::string newStatus = active.empty() ? "BENCH" : "ALLOCATED";
        if (employee.status != newStatus) {
            employeeRepository_->setStatus(employee.employeeId, newStatus);
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
        }
    }
}

void SchedulerService::markMissedTimesheets() {
    const auto employees  = employeeRepository_->findAllActive();
    const auto pastMondays = previousMondays(4);

    for (const auto& employee : employees) {
        const auto allocations = allocationRepository_->findActiveByEmployeeId(employee.employeeId);
        if (allocations.empty()) {
            continue;
        }

        for (const auto& weekStart : pastMondays) {
            const auto existing = timesheetRepository_->findByEmployeeAndWeek(employee.employeeId, weekStart);
            if (!existing.has_value()) {
                timesheetRepository_->createMissed(employee.employeeId, weekStart);
            }
        }
    }
}

std::string SchedulerService::todayString() {
    const std::time_t now = std::time(nullptr);
    std::tm nowTm = {};
    localtime_r(&now, &nowTm);
    std::ostringstream out;
    out << std::put_time(&nowTm, "%Y-%m-%d");
    return out.str();
}

std::string SchedulerService::lastMondayString() {
    const std::time_t now = std::time(nullptr);
    std::tm nowTm = {};
    localtime_r(&now, &nowTm);
    const int daysToSubtract = (nowTm.tm_wday == 0) ? 6 : (nowTm.tm_wday - 1);
    const std::time_t lastMonday = now - (daysToSubtract * 86400);
    std::tm mondayTm = {};
    localtime_r(&lastMonday, &mondayTm);
    std::ostringstream out;
    out << std::put_time(&mondayTm, "%Y-%m-%d");
    return out.str();
}

std::vector<std::string> SchedulerService::previousMondays(int count) {
    std::vector<std::string> result;
    const std::time_t now = std::time(nullptr);
    std::tm nowTm = {};
    localtime_r(&now, &nowTm);
    const int daysToLastMonday = (nowTm.tm_wday == 0) ? 6 : (nowTm.tm_wday - 1);

    for (int week = 1; week <= count; ++week) {
        const std::time_t mondayTime = now - ((daysToLastMonday + (week - 1) * 7) * 86400);
        std::tm mondayTm = {};
        localtime_r(&mondayTime, &mondayTm);
        std::ostringstream out;
        out << std::put_time(&mondayTm, "%Y-%m-%d");
        result.push_back(out.str());
    }
    return result;
}
