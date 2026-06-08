#include "AdminMenuScreen.hpp"
#include "AdminEmployeeScreen.hpp"
#include "AdminProjectScreen.hpp"
#include "AdminUsersScreen.hpp"
#include "AdminConfigScreen.hpp"
#include "../core/AppSession.hpp"
#include "../core/ConsoleUtil.hpp"

#include <iostream>

static void showAdminAllocations(const ApiClient& api) {
    while (true) {
        ConsoleUtil::clearScreen();
        ConsoleUtil::printHeader("ALL ALLOCATIONS");

        const auto resp = api.get("/api/admin/employees", AppSession::get().token);
        if (!resp.success) {
            ConsoleUtil::printError(resp.errorMessage);
            ConsoleUtil::pause();
            return;
        }

        const auto& employees = resp.body.value("data", nlohmann::json::array());

        std::cout << ConsoleUtil::col("Employee", 20)
                  << ConsoleUtil::col("Project", 22)
                  << ConsoleUtil::col("%", 5)
                  << ConsoleUtil::col("From", 12)
                  << "To\n";
        ConsoleUtil::printSeparator();

        int total = 0;
        for (const auto& emp : employees) {
            const std::string empName = emp.value("fullName", "");
            if (!emp.contains("allocations")) continue;
            for (const auto& alloc : emp["allocations"]) {
                if (!alloc.value("isActive", false)) continue;
                std::cout << ConsoleUtil::col(empName, 20)
                          << ConsoleUtil::col(alloc.value("projectName", ""), 22)
                          << ConsoleUtil::col(std::to_string(alloc.value("allocationPercentage", 0)) + "%", 5)
                          << ConsoleUtil::col(ConsoleUtil::fmtDate(alloc.value("fromDate", "")), 12)
                          << ConsoleUtil::fmtDate(alloc.value("toDate", "")) << "\n";
                ++total;
            }
        }

        ConsoleUtil::printSeparator();
        std::cout << "Total Active Allocations: " << total << "\n";

        std::cout << "\n[B] Back\n\nOption: ";
        std::string opt;
        std::getline(std::cin, opt);
        if (opt == "b" || opt == "B") return;
    }
}

void showAdminMenu(const ApiClient& api) {
    while (true) {
        ConsoleUtil::clearScreen();
        ConsoleUtil::printHeader(
            "ADMIN PANEL",
            "Welcome, " + AppSession::get().fullName + "  |  " + ConsoleUtil::currentDateTime()
        );

        std::cout << "1. Manage Employees\n"
                  << "2. Manage Projects\n"
                  << "3. View All Allocations\n"
                  << "4. Manage Users\n"
                  << "5. System Configuration\n"
                  << "6. Logout\n"
                  << "\nEnter option: ";

        std::string opt;
        std::getline(std::cin, opt);

        if (opt == "1") showAdminEmployeeMenu(api);
        else if (opt == "2") showAdminProjectMenu(api);
        else if (opt == "3") showAdminAllocations(api);
        else if (opt == "4") showAdminUsersMenu(api);
        else if (opt == "5") showAdminConfigMenu(api);
        else if (opt == "6") {
            api.post("/api/auth/logout", {}, AppSession::get().token);
            AppSession::get().clear();
            return;
        }
    }
}
