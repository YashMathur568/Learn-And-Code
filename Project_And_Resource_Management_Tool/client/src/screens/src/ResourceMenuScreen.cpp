#include "ResourceMenuScreen.hpp"
#include "ResourceTimesheetScreen.hpp"
#include "ResourceAllocationsScreen.hpp"
#include "AppSession.hpp"
#include "ConsoleUtil.hpp"

#include <iostream>


static std::string getMissedReminder(const ApiClient& api) {
    const std::string weekStart = ConsoleUtil::lastMonday();


    const auto allocResp = api.get("/api/resource/allocations", AppSession::get().token);
    if (!allocResp.success) return "";

    bool hasActiveAllocations = false;
    for (const auto& allocation : allocResp.body.value("data", nlohmann::json::array())) {
        if (allocation.value("isActive", false)) {
            hasActiveAllocations = true;
            break;
        }
    }


    if (!hasActiveAllocations) return "";


    const auto resp = api.get("/api/resource/timesheets", AppSession::get().token);
    if (!resp.success) return "";

    for (const auto& timesheet : resp.body.value("data", nlohmann::json::array())) {
        if (timesheet.value("weekStart", "") == weekStart) {
            const std::string status = timesheet.value("status", "");
            if (status == "SUBMITTED") return "";
        }
    }
    return "Reminder: Timesheet for week " + ConsoleUtil::fmtDate(weekStart) + " has not been submitted.";
}

void showResourceMenu(const ApiClient& api) {
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

        std::string selectedOption;
        std::getline(std::cin, selectedOption);

        if (selectedOption == "1") showSubmitTimesheet(api);
        else if (selectedOption == "2") showMyTimesheets(api);
        else if (selectedOption == "3") showMyAllocations(api);
        else if (selectedOption == "4") {
            api.post("/api/auth/logout", {}, AppSession::get().token);
            AppSession::get().clear();
            return;
        }
        else {
            ConsoleUtil::printError("Invalid option. Please enter a number from the menu.");
            ConsoleUtil::pause();
        }
    }
}
