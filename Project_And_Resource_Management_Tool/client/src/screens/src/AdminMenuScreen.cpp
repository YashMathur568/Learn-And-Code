#include "AdminMenuScreen.hpp"
#include "AdminResourceScreen.hpp"
#include "AdminProjectScreen.hpp"
#include "AdminUsersScreen.hpp"
#include "AdminConfigScreen.hpp"
#include "AppSession.hpp"
#include "ConsoleUtil.hpp"

#include <iostream>

static void showAdminAllocations(const ApiClient& api) {
    while (true) {
        ConsoleUtil::clearScreen();
        ConsoleUtil::printHeader("ALL ALLOCATIONS");

        const auto resp = api.get("/api/admin/resources", AppSession::get().token);
        if (!resp.success) {
            ConsoleUtil::printError(resp.errorMessage);
            ConsoleUtil::pause();
            return;
        }

        const auto& resources = resp.body.value("data", nlohmann::json::array());

        std::cout << ConsoleUtil::col("Resource", 20)
                  << ConsoleUtil::col("Project", 22)
                  << ConsoleUtil::col("%", 5)
                  << ConsoleUtil::col("From", 12)
                  << "To\n";
        ConsoleUtil::printSeparator();

        int total = 0;
        for (const auto& employee : resources) {
            const std::string resName = employee.value("fullName", "");
            if (!employee.contains("allocations")) continue;
            for (const auto& allocation : employee["allocations"]) {
                if (!allocation.value("isActive", false)) continue;
                std::cout << ConsoleUtil::col(resName, 20)
                          << ConsoleUtil::col(allocation.value("projectName", ""), 22)
                          << ConsoleUtil::col(std::to_string(allocation.value("allocationPercentage", 0)) + "%", 5)
                          << ConsoleUtil::col(ConsoleUtil::fmtDate(allocation.value("fromDate", "")), 12)
                          << ConsoleUtil::fmtDate(allocation.value("toDate", "")) << "\n";
                ++total;
            }
        }

        ConsoleUtil::printSeparator();
        std::cout << "Total Active Allocations: " << total << "\n";

        std::cout << "\n[B] Back\n\nOption: ";
        std::string selectedOption;
        std::getline(std::cin, selectedOption);
        if (selectedOption == "b" || selectedOption == "B") return;
        ConsoleUtil::printError("Press B to go back.");
        ConsoleUtil::pause();
    }
}

void showAdminMenu(const ApiClient& api) {
    while (true) {
        ConsoleUtil::clearScreen();
        ConsoleUtil::printHeader(
            "ADMIN PANEL",
            "Welcome, " + AppSession::get().fullName + "  |  " + ConsoleUtil::currentDateTime()
        );

        std::cout << "1. Manage Resources\n"
                  << "2. Manage Projects\n"
                  << "3. View All Allocations\n"
                  << "4. Manage Users\n"
                  << "5. System Configuration\n"
                  << "6. Logout\n"
                  << "\nEnter option: ";

        std::string selectedOption;
        std::getline(std::cin, selectedOption);

        if (selectedOption == "1") showAdminResourceMenu(api);
        else if (selectedOption == "2") showAdminProjectMenu(api);
        else if (selectedOption == "3") showAdminAllocations(api);
        else if (selectedOption == "4") showAdminUsersMenu(api);
        else if (selectedOption == "5") showAdminConfigMenu(api);
        else if (selectedOption == "6") {
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
