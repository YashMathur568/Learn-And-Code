#include "ManagerTimesheetsScreen.hpp"
#include "../core/AppSession.hpp"
#include "../core/ConsoleUtil.hpp"

#include <iostream>

void showManagerTimesheets(const ApiClient& api) {
    while (true) {
        ConsoleUtil::clearScreen();
        ConsoleUtil::printHeader("TIMESHEETS — MY TEAM");

        const std::string weekInput = ConsoleUtil::promptInput(
            "Filter by week (DD-MM-YYYY) or Enter for current week: "
        );

        const std::string weekStart = weekInput.empty()
            ? ConsoleUtil::lastMonday()
            : ConsoleUtil::toIsoDate(weekInput);

        const auto resp = api.get(
            "/api/manager/timesheets?weekStart=" + weekStart,
            AppSession::get().token
        );

        if (!resp.success) { ConsoleUtil::printError(resp.errorMessage); ConsoleUtil::pause(); return; }

        ConsoleUtil::clearScreen();
        ConsoleUtil::printHeader("TIMESHEETS — MY TEAM");
        std::cout << "Week: " << ConsoleUtil::fmtDate(weekStart) << "\n\n";

        const auto& data = resp.body.value("data", nlohmann::json::array());

        std::cout << ConsoleUtil::col("Employee",   20)
                  << ConsoleUtil::col("Total Hrs",  10)
                  << "Status\n";
        ConsoleUtil::printSeparator();

        if (data.empty()) {
            ConsoleUtil::printInfo("No timesheet data for this week.");
        } else {
            for (const auto& ts : data) {
                const std::string status = ts.value("status", "");
                const int hours = ts.value("totalHours", 0);
                const std::string statusDisplay = (status == "MISSED") ? "MISSED ⚠" : status;
                std::cout << ConsoleUtil::col(ts.value("employeeName", ""), 20)
                          << ConsoleUtil::col(std::to_string(hours) + " hrs", 10)
                          << statusDisplay << "\n";
            }
        }

        ConsoleUtil::printSeparator();
        std::cout << "\n[B] Back\nOption: ";
        std::string opt;
        std::getline(std::cin, opt);
        if (opt == "b" || opt == "B") return;
    }
}
