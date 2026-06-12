#include "TimesheetService.hpp"
#include "../utils/AppException.hpp"
#include "../utils/ConfigLoader.hpp"

#include <sstream>
#include <iomanip>
#include <ctime>
#include <numeric>

TimesheetService::TimesheetService(
    std::shared_ptr<ITimesheetRepository>  timesheetRepository,
    std::shared_ptr<IAllocationRepository> allocationRepository,
    std::shared_ptr<IEmployeeRepository>   employeeRepository
) : timesheetRepository(std::move(timesheetRepository)),
    allocationRepository(std::move(allocationRepository)),
    employeeRepository(std::move(employeeRepository)) {}

bool TimesheetService::isMondayDate(const std::string& dateStr) {
    std::tm timeStruct = {};
    std::istringstream stream(dateStr);
    stream >> std::get_time(&timeStruct, "%Y-%m-%d");
    if (stream.fail()) {
        return false;
    }
    timeStruct.tm_hour = 12;
    timeStruct.tm_min  = 0;
    timeStruct.tm_sec  = 0;
    std::mktime(&timeStruct);
    return timeStruct.tm_wday == 1;
}

bool TimesheetService::isWeekInFuture(const std::string& weekStart) {
    std::tm weekTm = {};
    std::istringstream stream(weekStart);
    stream >> std::get_time(&weekTm, "%Y-%m-%d");
    if (stream.fail()) {
        return true;
    }
    weekTm.tm_hour = 12;
    weekTm.tm_min  = 0;
    weekTm.tm_sec  = 0;
    const std::time_t weekTime = std::mktime(&weekTm);

    const std::time_t now = std::time(nullptr);
    std::tm nowTm = {};
    localtime_r(&now, &nowTm);
    nowTm.tm_hour = 12;
    nowTm.tm_min  = 0;
    nowTm.tm_sec  = 0;
    const std::time_t today = std::mktime(&nowTm);

    return weekTime > today;
}

TimesheetWithEntries TimesheetService::submitTimesheet(int userId, const SubmitTimesheetRequest& request) {
    // Check if account is frozen
    if (employeeRepository->isFrozen(userId)) {
        throw ValidationException("Your timesheet submission access has been frozen due to missed submissions. Contact your manager to restore access.");
    }

    if (request.entries.empty()) {
        throw ValidationException("Timesheet must contain at least one entry.");
    }
    if (!isMondayDate(request.weekStart)) {
        throw ValidationException("weekStart must be a Monday in YYYY-MM-DD format.");
    }
    if (isWeekInFuture(request.weekStart)) {
        throw ValidationException("Cannot submit a timesheet for a future week.");
    }

    const auto existing = timesheetRepository->findByUserAndWeek(userId, request.weekStart);
    if (existing.has_value() && existing->status == "SUBMITTED") {
        throw ConflictException("A timesheet for this week has already been submitted.");
    }
    // If existing status is MISSED, we allow retroactive submission (replaces it)

    int totalHours = 0;
    for (const auto& entryRequest : request.entries) {
        if (entryRequest.projectId <= 0) {
            throw ValidationException("Each entry must have a valid projectId.");
        }
        if (entryRequest.hours <= 0) {
            throw ValidationException("Entry hours must be greater than zero.");
        }
        if (!allocationRepository->isActivelyAllocated(userId, entryRequest.projectId)) {
            throw ValidationException(
                "Employee is not actively allocated to project " +
                std::to_string(entryRequest.projectId) + "."
            );
        }
        totalHours += entryRequest.hours;
    }

    const int maxWeeklyHours = ConfigLoader::getInstance().getConfig().maxWeeklyHours;
    if (totalHours > maxWeeklyHours) {
        throw ValidationException(
            "Total hours (" + std::to_string(totalHours) +
            ") exceed the weekly cap of " + std::to_string(maxWeeklyHours) + " hours."
        );
    }

    // If a MISSED record exists, update it in-place; otherwise create fresh
    int timesheetId = 0;
    if (existing.has_value() && existing->status == "MISSED") {
        timesheetId = existing->timesheetId;
        timesheetRepository->updateStatus(timesheetId, "SUBMITTED");
    } else {
        Timesheet timesheet;
        timesheet.userId    = userId;
        timesheet.weekStart = request.weekStart;
        timesheet.status    = "SUBMITTED";
        timesheetId = timesheetRepository->create(timesheet);
    }

    TimesheetWithEntries result;
    result.timesheet = timesheetRepository->findById(timesheetId).value();

    for (const auto& entryRequest : request.entries) {
        TimesheetEntry entry;
        entry.timesheetId   = timesheetId;
        entry.projectId     = entryRequest.projectId;
        entry.hours         = entryRequest.hours;
        entry.activityTags  = entryRequest.activityTags;
        timesheetRepository->addEntry(entry);
    }

    result.entries = timesheetRepository->getEntries(timesheetId);
    return result;
}

std::vector<TimesheetWithEntries> TimesheetService::getByUserId(int userId) {
    const auto timesheets = timesheetRepository->findByUserId(userId);
    std::vector<TimesheetWithEntries> result;
    result.reserve(timesheets.size());
    for (const auto& timesheet : timesheets) {
        TimesheetWithEntries record;
        record.timesheet = timesheet;
        record.entries   = timesheetRepository->getEntries(timesheet.timesheetId);
        result.push_back(std::move(record));
    }
    return result;
}

std::vector<TimesheetWithEntries> TimesheetService::getTeamTimesheets(
    int managerUserId, const std::string& weekStart
) {
    const auto timesheets = timesheetRepository->findByManagerTeam(managerUserId, weekStart);
    std::vector<TimesheetWithEntries> result;
    result.reserve(timesheets.size());
    for (const auto& timesheet : timesheets) {
        TimesheetWithEntries record;
        record.timesheet = timesheet;
        record.entries   = timesheetRepository->getEntries(timesheet.timesheetId);
        result.push_back(std::move(record));
    }
    return result;
}
