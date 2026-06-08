#include "ManagerMenuScreen.hpp"
#include "ManagerDashboardScreen.hpp"
#include "ManagerAllocateScreen.hpp"
#include "ManagerProjectsScreen.hpp"
#include "ManagerTimesheetsScreen.hpp"
#include "ManagerAIScreen.hpp"
#include "../core/AppSession.hpp"
#include "../core/ConsoleUtil.hpp"

#include <iostream>

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
                  << "5. AI Assistant\n"
                  << "6. Logout\n"
                  << "\nEnter option: ";

        std::string opt;
        std::getline(std::cin, opt);

        if (opt == "1") showManagerDashboard(api);
        else if (opt == "2") showManagerAllocate(api);
        else if (opt == "3") showManagerProjects(api);
        else if (opt == "4") showManagerTimesheets(api);
        else if (opt == "5") showManagerAI(api);
        else if (opt == "6") {
            api.post("/api/auth/logout", {}, AppSession::get().token);
            AppSession::get().clear();
            return;
        }
    }
}
