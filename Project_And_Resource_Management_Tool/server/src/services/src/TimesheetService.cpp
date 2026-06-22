#include "TimesheetService.hpp"
#include "AppException.hpp"
#include "ConfigLoader.hpp"

#include <sstream>
#include <iomanip>
#include <ctime>
#include <numeric>

TimesheetService::TimesheetService(
    std::shared_ptr<ITimesheetRepository>  timesheetRepository,
    std::shared_ptr<IAllocationRepository> allocationRepository,
    std::shared_ptr<IResourceRepository>   resourceRepository
) : timesheetRepository(std::move(timesheetRepository)),
    allocationRepository(std::move(allocationRepository)),
    resourceRepository(std::move(resourceRepository)) {}

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
    std::tm weekTimeStructure = {};
    std::istringstream stream(weekStart);
    stream >> std::get_time(&weekTimeStructure, "%Y-%m-%d");
    if (stream.fail()) {
        return true;
    }
    weekTimeStructure.tm_hour = 12;
    weekTimeStructure.tm_min  = 0;
    weekTimeStructure.tm_sec  = 0;
    const std::time_t weekTime = std::mktime(&weekTimeStructure);

    const std::time_t now = std::time(nullptr);
    std::tm nowTimeStructure = {};
    localtime_r(&now, &nowTimeStructure);
    nowTimeStructure.tm_hour = 12;
    nowTimeStructure.tm_min  = 0;
    nowTimeStructure.tm_sec  = 0;
    const std::time_t today = std::mktime(&nowTimeStructure);

    return weekTime > today;
}

std::string TimesheetService::getCurrentWeekMonday() {
    const std::time_t now = std::time(nullptr);
    std::tm t = {};
    localtime_r(&now, &t);
    // tm_wday: 0=Sunday, 1=Monday ... 6=Saturday
    int daysFromMonday = (t.tm_wday == 0) ? 6 : (t.tm_wday - 1);
    t.tm_mday -= daysFromMonday;
    t.tm_hour = 12; t.tm_min = 0; t.tm_sec = 0;
    std::mktime(&t);
    char buf[11];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d", &t);
    return std::string(buf);
}

TimesheetWithEntries TimesheetService::submitTimesheet(int userId, const SubmitTimesheetRequest& request) {

    if (resourceRepository->isFrozen(userId)) {
        throw ValidationException("Your timesheet submission access has been frozen due to missed submissions. Contact your manager to restore access.");
    }

    if (request.entries.empty()) {
        throw ValidationException("Timesheet must contain at least one entry.");
    }
    if (!isMondayDate(request.weekStart)) {
        throw ValidationException("weekStart must be allocation Monday in YYYY-MM-DD format.");
    }
    if (isWeekInFuture(request.weekStart)) {
        throw ValidationException("Cannot submit allocation timesheet for allocation future week.");
    }

    const auto existing = timesheetRepository->findByUserAndWeek(userId, request.weekStart);
    if (existing.has_value() && existing->status == "SUBMITTED") {
        throw ConflictException("A timesheet for this week has already been submitted.");
    }


    int totalHours = 0;
    for (const auto& entryRequest : request.entries) {
        if (entryRequest.projectId <= 0) {
            throw ValidationException("Each entry must have a valid projectId.");
        }
        if (entryRequest.hours <= 0) {
            throw ValidationException("Entry hours must be greater than zero.");
        }
        if (!allocationRepository->wasAllocatedDuringWeek(userId, entryRequest.projectId, request.weekStart)) {
            throw ValidationException(
                "Resource was not allocated to project " +
                std::to_string(entryRequest.projectId) + " during week " + request.weekStart + "."
            );
        }

        // Only block ON_HOLD projects for the current week.
        // For past missed weeks the project may have been active at the time,
        // so we allow filling regardless of current project status.
        const std::string currentWeekMonday = getCurrentWeekMonday();
        if (request.weekStart >= currentWeekMonday) {
            const std::string projectStatus = allocationRepository->getProjectStatus(entryRequest.projectId);
            if (projectStatus == "ON_HOLD") {
                throw ValidationException(
                    "Project " + std::to_string(entryRequest.projectId) +
                    " is currently on hold. Timesheet submission is not allowed for on-hold projects."
                );
            }
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
