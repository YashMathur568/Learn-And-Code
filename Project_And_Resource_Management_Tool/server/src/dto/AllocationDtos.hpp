#pragma once

#include "../models/Allocation.hpp"
#include "../models/Timesheet.hpp"
#include "../models/TimesheetEntry.hpp"
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

struct CreateAllocationRequest {
    int         userId{0};
    int         projectId{0};
    int         utilisation{0};
    std::string fromDate;
    std::string toDate;

    static CreateAllocationRequest fromJson(const nlohmann::json& body) {
        CreateAllocationRequest request;
        request.userId      = body.at("userId").get<int>();
        request.projectId   = body.at("projectId").get<int>();
        request.utilisation = body.at("utilisation").get<int>();
        request.fromDate    = body.at("fromDate").get<std::string>();
        request.toDate      = body.at("toDate").get<std::string>();
        return request;
    }
};

struct TimesheetEntryRequest {
    int         projectId{0};
    int         hours{0};
    std::string activityTags;
};

struct SubmitTimesheetRequest {
    std::string                        weekStart;
    std::vector<TimesheetEntryRequest> entries;

    static SubmitTimesheetRequest fromJson(const nlohmann::json& body) {
        SubmitTimesheetRequest request;
        request.weekStart = body.at("weekStart").get<std::string>();
        for (const auto& entry : body.at("entries")) {
            TimesheetEntryRequest entryRequest;
            entryRequest.projectId    = entry.at("projectId").get<int>();
            entryRequest.hours        = entry.at("hours").get<int>();
            entryRequest.activityTags = entry.value("activityTags", "");
            request.entries.push_back(entryRequest);
        }
        return request;
    }
};

struct TimesheetWithEntries {
    Timesheet                   timesheet;
    std::vector<TimesheetEntry> entries;
};

inline nlohmann::json allocationToJson(const Allocation& allocation) {
    return {
        {"allocationId",         allocation.allocationId},
        {"userId",               allocation.userId},
        {"projectId",            allocation.projectId},
        {"projectName",          allocation.projectName},
        {"allocationPercentage", allocation.utilisation},
        {"fromDate",             allocation.fromDate},
        {"toDate",               allocation.toDate},
        {"isActive",             allocation.isActive}
    };
}

inline nlohmann::json timesheetToJson(const Timesheet& timesheet) {
    return {
        {"timesheetId",  timesheet.timesheetId},
        {"userId",       timesheet.userId},
        {"weekStart",    timesheet.weekStart},
        {"status",       timesheet.status},
        {"submittedAt",  timesheet.submittedAt}
    };
}

inline nlohmann::json entryToJson(const TimesheetEntry& entry) {
    return {
        {"entryId",      entry.entryId},
        {"timesheetId",  entry.timesheetId},
        {"projectId",    entry.projectId},
        {"hours",        entry.hours},
        {"activityTags", entry.activityTags}
    };
}
