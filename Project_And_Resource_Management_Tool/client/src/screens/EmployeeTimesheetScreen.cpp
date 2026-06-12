#include "EmployeeTimesheetScreen.hpp"
#include "../core/AppSession.hpp"
#include "../core/ConsoleUtil.hpp"

#include <iostream>
#include <vector>

static const std::vector<std::string> ACTIVITY_TAGS = {
    "Backend API Development",
    "Microservices / Architecture",
    "Database Design & Queries",
    "WebSocket / Real-time Features",
    "Frontend Development",
    "Code Review / Mentoring",
    "Bug Fixing",
    "DevOps / Deployment",
    "Testing & QA",
    "Documentation"
};

static std::vector<std::string> selectActivityTags() {
    std::cout << "\nActivity Tags:\n";
    for (size_t tagIdx = 0; tagIdx < ACTIVITY_TAGS.size(); ++tagIdx) {
        std::cout << "  " << ConsoleUtil::col(std::to_string(tagIdx + 1) + ".", 4)
                  << ACTIVITY_TAGS[tagIdx] << "\n";
    }
    std::cout << "  11. Other (type manually)\n\n";
    std::cout << "Select tags (comma-separated, e.g. 1,3,7): ";

    std::string input;
    std::getline(std::cin, input);

    std::vector<std::string> tags;
    std::string token;
    for (char ch : input + ",") {
        if (ch == ',') {
            while (!token.empty() && token.front() == ' ') token.erase(token.begin());
            while (!token.empty() && token.back() == ' ')  token.pop_back();
            if (!token.empty()) {
                int idx = 0;
                try { idx = std::stoi(token); } catch (...) { tags.push_back(token); token.clear(); continue; }
                if (idx == 11) {
                    const std::string custom = ConsoleUtil::promptInput("Custom tag: ");
                    if (!custom.empty()) tags.push_back(custom);
                } else if (idx >= 1 && idx <= static_cast<int>(ACTIVITY_TAGS.size())) {
                    tags.push_back(ACTIVITY_TAGS[idx - 1]);
                }
            }
            token.clear();
        } else {
            token += ch;
        }
    }
    return tags;
}

void showSubmitTimesheet(const ApiClient& api) {
    ConsoleUtil::clearScreen();
    ConsoleUtil::printHeader("SUBMIT TIMESHEET");

    const std::string empName = AppSession::get().fullName;
    std::cout << "Employee  : " << empName << "\n\n";

    const std::string dateInput = ConsoleUtil::promptInput(
        "Week Start (DD-MM-YYYY) or Enter for last Monday: "
    );
    const std::string weekStart = dateInput.empty()
        ? ConsoleUtil::lastMonday()
        : ConsoleUtil::toWeekMonday(ConsoleUtil::toIsoDate(dateInput));
    if (!dateInput.empty() && weekStart.empty()) {
        ConsoleUtil::printError("Invalid date. Use DD-MM-YYYY.");
        ConsoleUtil::pause(); return;
    }

    const std::string today = ConsoleUtil::todayIso();
    if (weekStart > today) {
        ConsoleUtil::printError(
            "Week " + ConsoleUtil::fmtDate(weekStart) +
            " is in the future — timesheets can only be submitted for past or current weeks."
        );
        ConsoleUtil::pause(); return;
    }

    std::cout << "\nChecking your active allocations for " << ConsoleUtil::fmtDate(weekStart) << "...\n";

    const auto allocResp = api.get("/api/employee/allocations", AppSession::get().token);
    if (!allocResp.success) { ConsoleUtil::printError(allocResp.errorMessage); ConsoleUtil::pause(); return; }

    const auto& allocs = allocResp.body.value("data", nlohmann::json::array());

    struct EntryInput {
        int         projectId;
        std::string projectName;
        int         hours;
        std::vector<std::string> tags;
    };

    std::vector<EntryInput> entries;
    int totalHrs = 0;
    int projNum  = 1;

    for (const auto& allocation : allocs) {
        if (!allocation.value("isActive", false)) continue;
        const std::string from = allocation.value("fromDate", "");
        const std::string to   = allocation.value("toDate",   "");
        if (weekStart < from || weekStart > to) continue;

        const int pct      = allocation.value("allocationPercentage", 0);
        const int maxHrs   = pct * 40 / 100;

        ConsoleUtil::clearScreen();
        ConsoleUtil::printHeader("SUBMIT TIMESHEET");
        ConsoleUtil::printSeparator();
        std::cout << "PROJECT " << projNum++ << " — " << allocation.value("projectName", "") << "\n";
        std::cout << "Allocation: " << pct << "%   |   Expected: " << maxHrs << " hrs max\n";
        ConsoleUtil::printSeparator();

        const std::string hrsStr = ConsoleUtil::promptInput("Hours worked this week: ");
        int hours = 0;
        try { hours = std::stoi(hrsStr); } catch (...) {}

        const auto tags = selectActivityTags();

        entries.push_back({
            allocation.value("projectId", 0),
            allocation.value("projectName", ""),
            hours,
            tags
        });
        totalHrs += hours;
    }

    if (entries.empty()) {
        ConsoleUtil::printInfo("No allocations covering week " + ConsoleUtil::fmtDate(weekStart) + ".");

        // Find the earliest allocation from-date to hint when they can first submit
        std::string earliestFrom;
        for (const auto& alloc : allocs) {
            const std::string from = alloc.value("fromDate", "");
            if (!from.empty() && (earliestFrom.empty() || from < earliestFrom))
                earliestFrom = from;
        }
        if (!earliestFrom.empty() && earliestFrom > weekStart) {
            const std::string firstWeek = ConsoleUtil::toWeekMonday(earliestFrom);
            ConsoleUtil::printInfo(
                "Your allocation starts " + ConsoleUtil::fmtDate(earliestFrom) +
                " — first submittable week: " + ConsoleUtil::fmtDate(firstWeek) +
                " (you can submit from that Monday onwards)."
            );
        }
        ConsoleUtil::pause();
        return;
    }

    // Summary
    ConsoleUtil::clearScreen();
    ConsoleUtil::printHeader("TIMESHEET SUMMARY");
    std::cout << ConsoleUtil::col("Project",   24)
              << ConsoleUtil::col("Hrs",        6)
              << "Activity Tags\n";
    ConsoleUtil::printSeparator();
    for (const auto& entryItem : entries) {
        std::string tagStr;
        for (const auto& tag : entryItem.tags) { if (!tagStr.empty()) tagStr += ", "; tagStr += tag; }
        std::cout << ConsoleUtil::col(ConsoleUtil::trunc(entryItem.projectName, 23), 24)
                  << ConsoleUtil::col(std::to_string(entryItem.hours), 6)
                  << ConsoleUtil::trunc(tagStr, 40) << "\n";
    }
    ConsoleUtil::printSeparator();
    std::cout << "Total  " << totalHrs << " hrs / 40 hrs max\n";

    if (!ConsoleUtil::confirm("Submit timesheet?")) {
        ConsoleUtil::printInfo("Cancelled."); ConsoleUtil::pause(); return;
    }

    // Build payload
    nlohmann::json entriesJson = nlohmann::json::array();
    for (const auto& entryItem : entries) {
        std::string tagStr;
        for (const auto& tag : entryItem.tags) { if (!tagStr.empty()) tagStr += ","; tagStr += tag; }
        entriesJson.push_back({
            {"projectId",    entryItem.projectId},
            {"hours",        entryItem.hours},
            {"activityTags", tagStr}
        });
    }

    const auto resp = api.post("/api/employee/timesheets", {
        {"weekStart", weekStart},
        {"entries",   entriesJson}
    }, AppSession::get().token);

    if (!resp.success) ConsoleUtil::printError(resp.errorMessage);
    else               ConsoleUtil::printSuccess("Timesheet submitted. Status: SUBMITTED");
    ConsoleUtil::pause();
}

void showMyTimesheets(const ApiClient& api) {
    while (true) {
        ConsoleUtil::clearScreen();
        ConsoleUtil::printHeader("MY TIMESHEETS");

        const auto resp = api.get("/api/employee/timesheets", AppSession::get().token);
        if (!resp.success) { ConsoleUtil::printError(resp.errorMessage); ConsoleUtil::pause(); return; }

        const auto& data = resp.body.value("data", nlohmann::json::array());

        std::cout << ConsoleUtil::col("Week Start",  14)
                  << ConsoleUtil::col("Total Hrs",   12)
                  << "Status\n";
        ConsoleUtil::printSeparator();

        for (const auto& ts : data) {
            const std::string status = ts.value("status", "");
            const int hours = ts.value("totalHours", 0);
            const std::string display = (status == "MISSED") ? "MISSED ⚠" : status;
            std::cout << ConsoleUtil::col(ConsoleUtil::fmtDate(ts.value("weekStart", "")), 14)
                      << ConsoleUtil::col(std::to_string(hours) + " hrs", 12)
                      << display << "\n";
        }
        ConsoleUtil::printSeparator();

        std::cout << "\n[V] View week detail   [B] Back\nOption: ";
        std::string opt;
        std::getline(std::cin, opt);

        if (opt == "b" || opt == "B") return;

        if ((opt == "v" || opt == "V") && !data.empty()) {
            const std::string weekInput = ConsoleUtil::promptInput("Week (DD-MM-YYYY): ");
            const std::string weekIso   = ConsoleUtil::toWeekMonday(ConsoleUtil::toIsoDate(weekInput));

            for (const auto& ts : data) {
                if (ts.value("weekStart", "") != weekIso) continue;

                ConsoleUtil::clearScreen();
                std::cout << "── Week: " << ConsoleUtil::fmtDate(weekIso)
                          << " — Status: " << ts.value("status", "") << " ──\n\n";

                std::cout << ConsoleUtil::col("Project",  24)
                          << ConsoleUtil::col("Hrs",       6)
                          << "Activity Tags\n";
                ConsoleUtil::printSeparator();

                for (const auto& entry : ts.value("entries", nlohmann::json::array())) {
                    std::string tags;
                    for (const auto& tag : entry.value("activityTags", nlohmann::json::array())) {
                        if (!tags.empty()) tags += ", ";
                        tags += tag.get<std::string>();
                    }
                    std::cout << ConsoleUtil::col(ConsoleUtil::trunc(entry.value("projectName",""),23),24)
                              << ConsoleUtil::col(std::to_string(entry.value("hoursWorked",0)),6)
                              << ConsoleUtil::trunc(tags, 40) << "\n";
                }
                ConsoleUtil::printSeparator();
                std::cout << "Total: " << ts.value("totalHours", 0) << " hrs\n";
                ConsoleUtil::pause();
                break;
            }
        }
    }
}
