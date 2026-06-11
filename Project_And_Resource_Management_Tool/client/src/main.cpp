#include "core/AppSession.hpp"
#include "core/ConsoleUtil.hpp"
#include "api/ApiClient.hpp"
#include "screens/LoginScreen.hpp"
#include "screens/AdminMenuScreen.hpp"
#include "screens/ManagerMenuScreen.hpp"
#include "screens/EmployeeMenuScreen.hpp"

#include <iostream>
#include <cstdlib>
#ifdef _WIN32
#  include <windows.h>
#endif

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    const std::string baseUrl = "http://localhost:8080";
    const ApiClient   api(baseUrl);

    while (true) {
        ConsoleUtil::clearScreen();
        ConsoleUtil::printHeader(
            "PROJECT & RESOURCE MANAGEMENT TOOL",
            "Learn & Code — Final Project"
        );

        std::cout << "1. Login\n"
                  << "2. Exit\n"
                  << "\nEnter option: ";

        std::string opt;
        std::getline(std::cin, opt);

        if (opt == "2") {
            ConsoleUtil::clearScreen();
            std::cout << "Goodbye.\n";
            return EXIT_SUCCESS;
        }

        if (opt != "1") continue;

        const bool ok = showLoginScreen(api);
        if (!ok) continue;

        const std::string role = AppSession::get().role;
        if (role == "ADMIN")          showAdminMenu(api);
        else if (role == "MANAGER")   showManagerMenu(api);
        else if (role == "EMPLOYEE")  showEmployeeMenu(api);
        else {
            ConsoleUtil::printError("Unknown role: " + role);
            AppSession::get().clear();
            ConsoleUtil::pause();
        }
    }
}
