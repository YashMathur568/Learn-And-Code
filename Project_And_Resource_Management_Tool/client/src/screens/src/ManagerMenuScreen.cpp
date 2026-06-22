#include "ManagerMenuScreen.hpp"
#include "ManagerDashboardScreen.hpp"
#include "ManagerAllocateScreen.hpp"
#include "ManagerProjectsScreen.hpp"
#include "ManagerTimesheetsScreen.hpp"
#include "ManagerAIScreen.hpp"
#include "ApiClient.hpp"
#include "AppSession.hpp"
#include "ConsoleUtil.hpp"

#include <iostream>

static void restoreTimesheetAccess(const ApiClient& api) {
    ConsoleUtil::clearScreen();
    ConsoleUtil::printHeader("RESTORE TIMESHEET ACCESS");

    const auto respAll = api.get("/api/admin/resources", AppSession::get().token);
    if (!respAll.success) { ConsoleUtil::printError(respAll.errorMessage); ConsoleUtil::pause(); return; }

    const auto& allEmps = respAll.body.value("data", nlohmann::json::array());

    std::cout << ConsoleUtil::col("ID",   6)
              << ConsoleUtil::col("Name", 26)
              << "Frozen\n";
    ConsoleUtil::printSeparator();
    bool anyFrozen = false;
    for (const auto& employee : allEmps) {
        const bool frozen = employee.value("isFrozen", false);
        if (!frozen) continue;
        anyFrozen = true;
        std::cout << ConsoleUtil::col(std::to_string(employee.value("userId", 0)), 6)
                  << ConsoleUtil::col(employee.value("fullName", ""), 26)
                  << "[FROZEN]\n";
    }
    if (!anyFrozen) {
        ConsoleUtil::printInfo("No resources have frozen timesheet access.");
        ConsoleUtil::pause();
        return;
    }
    ConsoleUtil::printSeparator();

    const std::string idStr = ConsoleUtil::promptInput("Resource ID to restore (B to go back): ");
    if (idStr.empty() || idStr == "b" || idStr == "B") return;
    try { std::stoi(idStr); } catch (...) {
        ConsoleUtil::printError("Please enter a numeric ID.");
        ConsoleUtil::pause(); return;
    }

    const auto resp = api.put("/api/admin/resources/" + idStr + "/restore-access", {}, AppSession::get().token);
    if (!resp.success) ConsoleUtil::printError(resp.errorMessage);
    else               ConsoleUtil::printSuccess("Timesheet access restored.");
    ConsoleUtil::pause();
}

void showManagerMenu(const ApiClient& api) {
    while (true) {
        ConsoleUtil::clearScreen();
        ConsoleUtil::printHeader(
            "Welcome, " + AppSession::get().fullName + "!",
            ConsoleUtil::currentDateTime()
        );

        std::cout << "1. Resource Dashboard\n"
                  << "2. Allocate Resource\n"
                  << "3. My Projects\n"
                  << "4. Timesheets\n"
                  << "5. Restore Timesheet Access\n"
                  << "6. AI Assistant\n"
                  << "7. Logout\n"
                  << "\nEnter option: ";

        std::string selectedOption;
        std::getline(std::cin, selectedOption);

        if (selectedOption == "1") showManagerDashboard(api);
        else if (selectedOption == "2") showManagerAllocate(api);
        else if (selectedOption == "3") showManagerProjects(api);
        else if (selectedOption == "4") showManagerTimesheets(api);
        else if (selectedOption == "5") restoreTimesheetAccess(api);
        else if (selectedOption == "6") showManagerAI(api);
        else if (selectedOption == "7") {
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
