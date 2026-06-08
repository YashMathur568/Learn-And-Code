#include "EmployeeMenuScreen.hpp"
#include "EmployeeTimesheetScreen.hpp"
#include "EmployeeAllocationsScreen.hpp"
#include "../core/AppSession.hpp"
#include "../core/ConsoleUtil.hpp"

#include <iostream>

// Check for missing timesheet this week and return a reminder string if needed
static std::string getMissedReminder(const ApiClient& api) {
    const std::string weekStart = ConsoleUtil::lastMonday();
    const auto resp = api.get("/api/employee/timesheets", AppSession::get().token);
    if (!resp.success) return "";

    for (const auto& ts : resp.body.value("data", nlohmann::json::array())) {
        if (ts.value("weekStart", "") == weekStart) {
            const std::string status = ts.value("status", "");
            if (status == "SUBMITTED") return "";
        }
    }
    return "Reminder: Timesheet for week " + ConsoleUtil::fmtDate(weekStart) + " has not been submitted.";
}

void showEmployeeMenu(const ApiClient& api) {
    while (true) {
        ConsoleUtil::clearScreen();
        ConsoleUtil::printHeader(
            "Welcome, " + AppSession::get().fullName + "!",
            ConsoleUtil::currentDate()
        );

        const std::string reminder = getMissedReminder(api);
        if (!reminder.empty()) {
            ConsoleUtil::printWarning(reminder);
            std::cout << "\n";
        }

        ConsoleUtil::printSeparator();
        std::cout << "1. Submit Timesheet\n"
                  << "2. View My Timesheets\n"
                  << "3. View My Allocations\n"
                  << "4. Logout\n"
                  << "\nEnter option: ";

        std::string opt;
        std::getline(std::cin, opt);

        if (opt == "1") showSubmitTimesheet(api);
        else if (opt == "2") showMyTimesheets(api);
        else if (opt == "3") showMyAllocations(api);
        else if (opt == "4") {
            api.post("/api/auth/logout", {}, AppSession::get().token);
            AppSession::get().clear();
            return;
        }
    }
}
