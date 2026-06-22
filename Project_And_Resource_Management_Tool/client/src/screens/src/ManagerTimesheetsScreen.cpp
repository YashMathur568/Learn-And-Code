#include "ManagerTimesheetsScreen.hpp"
#include "AppSession.hpp"
#include "ConsoleUtil.hpp"

#include <iostream>

void showManagerTimesheets(const ApiClient& api) {
    while (true) {
        ConsoleUtil::clearScreen();
        ConsoleUtil::printHeader("TIMESHEETS — MY TEAM");

        std::cout << "1. View This Week\n"
                  << "2. Filter by Week\n"
                  << "3. Back\n"
                  << "\nOption: ";

        std::string selectedOption;
        std::getline(std::cin, selectedOption);

        if (selectedOption == "3" || selectedOption == "b" || selectedOption == "B") return;
        if (selectedOption != "1" && selectedOption != "2") {
            ConsoleUtil::printError("Invalid option. Please enter 1, 2, or 3.");
            ConsoleUtil::pause();
            continue;
        }

        std::string weekStart;
        if (selectedOption == "1") {
            weekStart = ConsoleUtil::lastMonday();
        } else {
            const std::string weekInput = ConsoleUtil::promptInput("Week (DD-MM-YYYY): ");
            if (weekInput.empty()) continue;
            const std::string parsed = ConsoleUtil::toIsoDate(weekInput);
            if (parsed.empty()) {
                ConsoleUtil::printError("Invalid date — please use DD-MM-YYYY format.");
                ConsoleUtil::pause();
                continue;
            }
            weekStart = ConsoleUtil::toWeekMonday(parsed);
        }

        const auto resp = api.get(
            "/api/manager/timesheets?weekStart=" + weekStart,
            AppSession::get().token
        );

        if (!resp.success) { ConsoleUtil::printError(resp.errorMessage); ConsoleUtil::pause(); continue; }

        ConsoleUtil::clearScreen();
        ConsoleUtil::printHeader("TIMESHEETS — MY TEAM");
        std::cout << "Week: " << ConsoleUtil::fmtDate(weekStart) << "\n\n";

        const auto& data = resp.body.value("data", nlohmann::json::array());

        std::cout << ConsoleUtil::col("Resource",   20)
                  << ConsoleUtil::col("Total Hrs",  10)
                  << "Status\n";
        ConsoleUtil::printSeparator();

        if (data.empty()) {
            ConsoleUtil::printInfo("No timesheet data for this week.");
        } else {
            int index = 1;
            for (const auto& timesheet : data) {
                const std::string status        = timesheet.value("status", "");
                const int         hours         = timesheet.value("totalHours", 0);
                const std::string statusDisplay = (status == "MISSED") ? "MISSED \u26a0" : status;
                std::cout << ConsoleUtil::col(std::to_string(index++) + ". " + ConsoleUtil::trunc(timesheet.value("resourceName", ""), 18), 20)
                          << ConsoleUtil::col(std::to_string(hours) + " hrs", 10)
                          << statusDisplay << "\n";
            }
        }

        ConsoleUtil::printSeparator();
        std::cout << "\nEnter row number to view detail, or [B] Back: ";
        std::string sel;
        std::getline(std::cin, sel);
        if (sel == "b" || sel == "B") continue;

        int selIdx = 0;
        try { selIdx = std::stoi(sel); } catch (...) {}
        if (selIdx < 1 || selIdx > static_cast<int>(data.size())) {
            ConsoleUtil::printError("Please enter a valid row number from the list.");
            ConsoleUtil::pause();
            continue;
        }

        const auto& timesheet = data[selIdx - 1];
        ConsoleUtil::clearScreen();
        std::cout << "\u2500\u2500 " << timesheet.value("resourceName", "") << "  |  Week: "
                  << ConsoleUtil::fmtDate(weekStart)
                  << "  \u2014  Status: " << timesheet.value("status", "") << " \u2500\u2500\n\n";

        std::cout << ConsoleUtil::col("Project",  24)
                  << ConsoleUtil::col("Hrs",       6)
                  << "Activity Tags\n";
        ConsoleUtil::printSeparator();

        for (const auto& entry : timesheet.value("entries", nlohmann::json::array())) {
            const std::string tags = entry.value("activityTags", "");
            std::cout << ConsoleUtil::col(ConsoleUtil::trunc(entry.value("projectName", ""), 23), 24)
                      << ConsoleUtil::col(std::to_string(entry.value("hours", 0)), 6)
                      << ConsoleUtil::trunc(tags, 40) << "\n";
        }
        ConsoleUtil::printSeparator();
        std::cout << "Total: " << timesheet.value("totalHours", 0) << " hrs\n";
        ConsoleUtil::pause();
    }
}
