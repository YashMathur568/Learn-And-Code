#include "AppSession.hpp"
#include "ConsoleUtil.hpp"
#include "ApiClient.hpp"
#include "LoginScreen.hpp"
#include "AdminMenuScreen.hpp"
#include "ManagerMenuScreen.hpp"
#include "ResourceMenuScreen.hpp"

#include <iostream>
#include <cstdlib>
#ifdef _WIN32
#  include <windows.h>
#endif

int main() {
#ifdef _WIN32

    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    HANDLE consoleOutputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD consoleMode = 0;
    GetConsoleMode(consoleOutputHandle, &consoleMode);
    SetConsoleMode(consoleOutputHandle, consoleMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif
    const std::string baseApiUrl = "http://localhost:8080";
    const ApiClient   api(baseApiUrl);

    while (true) {
        ConsoleUtil::clearScreen();
        ConsoleUtil::printHeader(
            "PROJECT & RESOURCE MANAGEMENT TOOL",
            "Learn & Code — Final Project"
        );

        std::cout << "1. Login\n"
                  << "2. Exit\n"
                  << "\nEnter option: ";

        std::string selectedOption;
        std::getline(std::cin, selectedOption);

        if (selectedOption == "2") {
            ConsoleUtil::clearScreen();
            std::cout << "Goodbye.\n";
            return EXIT_SUCCESS;
        }

        if (selectedOption != "1") {
            ConsoleUtil::printError("Invalid option. Please enter 1 or 2.");
            ConsoleUtil::pause();
            continue;
        }

        const bool loginSuccess = showLoginScreen(api);
        if (!loginSuccess) continue;

        const std::string role = AppSession::get().role;
        if (role == "ADMIN")          showAdminMenu(api);
        else if (role == "MANAGER")   showManagerMenu(api);
        else if (role == "RESOURCE")  showResourceMenu(api);
        else {
            ConsoleUtil::printError("Unknown role: " + role);
            AppSession::get().clear();
            ConsoleUtil::pause();
        }
    }
}
