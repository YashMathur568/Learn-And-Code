#include "ResourceTimesheetScreen.hpp"
#include "AppSession.hpp"
#include "ConsoleUtil.hpp"

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

    std::vector<std::string> tags;
    while (tags.empty()) {
        std::cout << "Select tags (comma-separated, e.g. 1,3,7): ";

        std::string input;
        std::getline(std::cin, input);

        std::string token;
        for (char ch : input + ",") {
            if (ch == ',') {
                while (!token.empty() && token.front() == ' ') token.erase(token.begin());
                while (!token.empty() && token.back() == ' ')  token.pop_back();
                if (!token.empty()) {
                    int index = 0;
                    try {
                        index = std::stoi(token);
                    } catch (...) {
                        std::cout << "  Invalid selection '" << token << "' ignored.\n";
                        token.clear();
                        continue;
                    }
                    if (index == 11) {
                        const std::string custom = ConsoleUtil::promptInput("Custom tag: ");
                        if (!custom.empty()) tags.push_back(custom);
                    } else if (index >= 1 && index <= static_cast<int>(ACTIVITY_TAGS.size())) {
                        tags.push_back(ACTIVITY_TAGS[index - 1]);
                    } else {
                        std::cout << "  " << index << " is out of range (1-11), ignored.\n";
                    }
                }
                token.clear();
            } else {
                token += ch;
            }
        }
        if (tags.empty()) {
            std::cout << "  At least one tag must be selected. Please try again.\n";
        }
    }
    return tags;
}

void showSubmitTimesheet(const ApiClient& api) {
    ConsoleUtil::clearScreen();
    ConsoleUtil::printHeader("SUBMIT TIMESHEET");

    const std::string resName = AppSession::get().fullName;
    std::cout << "Resource  : " << resName << "\n\n";

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


    const auto empResp = api.get("/api/resource/current", AppSession::get().token);
    if (empResp.success) {
        const auto& empData = empResp.body.contains("data") ? empResp.body["data"] : empResp.body;
        if (empData.value("isFrozen", false)) {
            ConsoleUtil::printError(
                "Your timesheet submission access has been frozen due to missed submissions.\n"
                "  Contact your manager to restore access."
            );
            ConsoleUtil::pause();
            return;
        }
    }

    const auto allocResp = api.get("/api/resource/allocations", AppSession::get().token);
    if (!allocResp.success) { ConsoleUtil::printError(allocResp.errorMessage); ConsoleUtil::pause(); return; }

    const auto& allocations = allocResp.body.value("data", nlohmann::json::array());

    struct EntryInput {
        int         projectId;
        std::string projectName;
        int         hours;
        std::vector<std::string> tags;
    };

    std::vector<EntryInput> entries;
    int totalHrs = 0;
    int projNum  = 1;

    for (const auto& allocation : allocations) {
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

        int hours = 0;
        while (true) {
            const std::string hrsStr = ConsoleUtil::promptInput("Hours worked this week (1-" + std::to_string(maxHrs) + "): ");
            bool valid = true;
            try { hours = std::stoi(hrsStr); } catch (...) { valid = false; }
            if (!valid || hours < 1 || hours > maxHrs) {
                std::cout << "  Please enter a number between 1 and " << maxHrs << ".\n";
                continue;
            }
            break;
        }

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


        std::string earliestFrom;
        for (const auto& allocation : allocations) {
            const std::string from = allocation.value("fromDate", "");
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

    const auto resp = api.post("/api/resource/timesheets", {
        {"weekStart", weekStart},
        {"entries",   entriesJson}
    }, AppSession::get().token);

    if (!resp.success) ConsoleUtil::printError(resp.errorMessage);
    else               ConsoleUtil::printSuccess("Timesheet submitted. Status: SUBMITTED");
    ConsoleUtil::pause();
}


static void showWeekDetail(const nlohmann::json& timesheet) {
    ConsoleUtil::clearScreen();
    std::cout << "\u2500\u2500 Week: " << ConsoleUtil::fmtDate(timesheet.value("weekStart", ""))
              << "  \u2014  Status: " << timesheet.value("status", "") << " \u2500\u2500\n\n";

    std::cout << ConsoleUtil::col("Project",  24)
              << ConsoleUtil::col("Hrs",       6)
              << "Activity Tags\n";
    ConsoleUtil::printSeparator();

    for (const auto& entry : timesheet.value("entries", nlohmann::json::array())) {
        std::string tags = entry.value("activityTags", "");
        std::cout << ConsoleUtil::col(ConsoleUtil::trunc(entry.value("projectName",""), 23), 24)
                  << ConsoleUtil::col(std::to_string(entry.value("hours", 0)), 6)
                  << ConsoleUtil::trunc(tags, 40) << "\n";
    }
    ConsoleUtil::printSeparator();
    std::cout << "Total: " << timesheet.value("totalHours", 0) << " hrs\n";
    ConsoleUtil::pause();
}


static void viewAllTimesheets(const ApiClient& api) {
    while (true) {
        ConsoleUtil::clearScreen();
        ConsoleUtil::printHeader("MY TIMESHEETS — ALL");

        const auto resp = api.get("/api/resource/timesheets", AppSession::get().token);
        if (!resp.success) { ConsoleUtil::printError(resp.errorMessage); ConsoleUtil::pause(); return; }

        const auto& data = resp.body.value("data", nlohmann::json::array());

        if (data.empty()) {
            ConsoleUtil::printInfo("No timesheets found.");
            ConsoleUtil::pause();
            return;
        }

        std::cout << ConsoleUtil::col("Week Start", 14)
                  << ConsoleUtil::col("Total Hrs",  12)
                  << "Status\n";
        ConsoleUtil::printSeparator();

        int index = 1;
        for (const auto& timesheet : data) {
            const std::string status  = timesheet.value("status", "");
            const int         hours   = timesheet.value("totalHours", 0);
            const std::string display = (status == "MISSED") ? "MISSED \u26a0" : status;
            std::cout << ConsoleUtil::col(std::to_string(index++) + ". " + ConsoleUtil::fmtDate(timesheet.value("weekStart", "")), 14)
                      << ConsoleUtil::col(std::to_string(hours) + " hrs", 12)
                      << display << "\n";
        }
        ConsoleUtil::printSeparator();

        std::cout << "\nEnter row number to view detail, or [B] Back: ";
        std::string selectedOption;
        std::getline(std::cin, selectedOption);

        if (selectedOption == "b" || selectedOption == "B") return;

        int sel = 0;
        try { sel = std::stoi(selectedOption); } catch (...) {}
        if (sel >= 1 && sel <= static_cast<int>(data.size()))
            showWeekDetail(data[sel - 1]);
        else {
            ConsoleUtil::printError("Please enter a valid row number from the list.");
            ConsoleUtil::pause();
        }
    }
}


static void filterByWeek(const ApiClient& api) {
    ConsoleUtil::clearScreen();
    ConsoleUtil::printHeader("MY TIMESHEETS — FILTER BY WEEK");

    const std::string weekInput = ConsoleUtil::promptInput("Week (DD-MM-YYYY): ");
    if (weekInput.empty()) return;
    const std::string weekIso = ConsoleUtil::toWeekMonday(ConsoleUtil::toIsoDate(weekInput));
    if (weekIso.empty()) { ConsoleUtil::printError("Invalid date."); ConsoleUtil::pause(); return; }

    const auto resp = api.get("/api/resource/timesheets", AppSession::get().token);
    if (!resp.success) { ConsoleUtil::printError(resp.errorMessage); ConsoleUtil::pause(); return; }

    const auto& data = resp.body.value("data", nlohmann::json::array());
    for (const auto& timesheet : data) {
        if (timesheet.value("weekStart", "") == weekIso) {
            showWeekDetail(timesheet);
            return;
        }
    }

    ConsoleUtil::printInfo("No timesheet found for week " + ConsoleUtil::fmtDate(weekIso) + ".");
    ConsoleUtil::pause();
}

void showMyTimesheets(const ApiClient& api) {
    while (true) {
        ConsoleUtil::clearScreen();
        ConsoleUtil::printHeader("MY TIMESHEETS");

        std::cout << "1. View All Timesheets\n"
                  << "2. Filter by Week\n"
                  << "3. Back\n"
                  << "\nOption: ";

        std::string selectedOption;
        std::getline(std::cin, selectedOption);

        if      (selectedOption == "1") viewAllTimesheets(api);
        else if (selectedOption == "2") filterByWeek(api);
        else if (selectedOption == "3" || selectedOption == "b" || selectedOption == "B") return;
        else {
            ConsoleUtil::printError("Invalid option. Please enter a number from the menu.");
            ConsoleUtil::pause();
        }
    }
}
