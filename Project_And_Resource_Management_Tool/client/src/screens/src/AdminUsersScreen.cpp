#include "AdminUsersScreen.hpp"
#include "AppSession.hpp"
#include "ConsoleUtil.hpp"

#include <iostream>


static void createUser(const ApiClient& api) {
    ConsoleUtil::clearScreen();
    ConsoleUtil::printHeader("CREATE USER ACCOUNT");

    const std::string fullName = ConsoleUtil::promptInput("Full Name         : ");
    if (fullName.empty() || fullName == "b" || fullName == "B") return;

    std::string email;
    while (true) {
        email = ConsoleUtil::promptInput("Email             : ");
        if (email == "b" || email == "B") return;
        const auto at = email.find('@');
        const auto dot = email.rfind('.');
        if (at == std::string::npos || at == 0 || at == email.size() - 1 ||
            dot == std::string::npos || dot <= at + 1 || dot == email.size() - 1) {
            ConsoleUtil::printError("Invalid email. Enter a valid address (e.g. user@example.com).");
            continue;
        }
        break;
    }

    const std::string username = ConsoleUtil::promptInput("Username          : ");
    const std::string tempPass = ConsoleUtil::promptPassword("Temporary Password: ");

    const std::vector<std::string> roles = {"ADMIN","MANAGER","RESOURCE"};
    std::string role;
    while (true) {
        std::cout << "Role: (1) Admin  (2) Manager  (3) Resource\n";
        const std::string roleOpt = ConsoleUtil::promptInput("Choice            : ");
        if (roleOpt == "b" || roleOpt == "B") return;
        if (roleOpt >= "1" && roleOpt <= "3") { role = roles[std::stoi(roleOpt) - 1]; break; }
        ConsoleUtil::printError("Invalid choice. Enter 1 (Admin), 2 (Manager), or 3 (Resource).");
    }

    nlohmann::json payload = {
        {"fullName",     fullName},
        {"email",        email},
        {"username",     username},
        {"tempPassword", tempPass},
        {"role",         role}
    };

    if (role == "RESOURCE") {
        const std::string dept  = ConsoleUtil::selectFromList("Department",  ConsoleUtil::DEPARTMENTS,  "");
        const std::string desig = ConsoleUtil::selectFromList("Designation", ConsoleUtil::DESIGNATIONS, "");
        payload["department"]   = dept;
        payload["designation"]  = desig;
    } else if (role == "MANAGER") {
        const std::string desig = ConsoleUtil::selectFromList("Designation", ConsoleUtil::DESIGNATIONS, "");
        payload["designation"]  = desig;

    }




    const auto resp = api.post("/api/admin/users", payload, AppSession::get().token);

    if (!resp.success) ConsoleUtil::printError(resp.errorMessage);
    else               ConsoleUtil::printSuccess("Account created. User must change password on first login.");
    ConsoleUtil::pause();
}


static void viewAllUsers(const ApiClient& api) {
    while (true) {
        ConsoleUtil::clearScreen();
        ConsoleUtil::printHeader("ALL USERS");

        const auto resp = api.get("/api/admin/users", AppSession::get().token);
        if (!resp.success) { ConsoleUtil::printError(resp.errorMessage); ConsoleUtil::pause(); return; }

        const auto& data = resp.body.value("data", nlohmann::json::array());

        std::cout << ConsoleUtil::col("ID",       5)
                  << ConsoleUtil::col("Username", 20)
                  << ConsoleUtil::col("Full Name",20)
                  << ConsoleUtil::col("Role",     12)
                  << "Status\n";
        ConsoleUtil::printSeparator();

        int active = 0, inactive = 0;
        for (const auto& user : data) {
            const bool isActive = user.value("isActive", false);
            if (isActive) ++active; else ++inactive;
            std::cout << ConsoleUtil::col(std::to_string(user.value("userId", 0)), 5)
                      << ConsoleUtil::col(user.value("username", ""), 20)
                      << ConsoleUtil::col(ConsoleUtil::trunc(user.value("fullName", ""), 19), 20)
                      << ConsoleUtil::col(user.value("role", ""), 12)
                      << (isActive ? "Active" : "Inactive") << "\n";
        }
        ConsoleUtil::printSeparator();
        std::cout << "Total: " << data.size()
                  << "   |   Active: " << active
                  << "   |   Inactive: " << inactive << "\n";

        std::cout << "\n[R] Reactivate   [B] Back\nOption: ";
        std::string selectedOption;
        std::getline(std::cin, selectedOption);

        if (selectedOption == "b" || selectedOption == "B") return;

        if (selectedOption == "role" || selectedOption == "R") {
            const std::string idStr = ConsoleUtil::promptInput("User ID to reactivate: ");
            if (idStr.empty()) continue;
            const auto reResp = api.put(
                "/api/admin/users/" + idStr + "/reactivate",
                {},
                AppSession::get().token
            );
            if (!reResp.success) ConsoleUtil::printError(reResp.errorMessage);
            else                 ConsoleUtil::printSuccess("Account reactivated.");
            ConsoleUtil::pause();
        }
        else {
            ConsoleUtil::printError("Invalid option. Press B to go back or R to reactivate allocation user.");
            ConsoleUtil::pause();
        }
    }
}


static void resetPassword(const ApiClient& api) {
    ConsoleUtil::clearScreen();
    ConsoleUtil::printHeader("RESET USER PASSWORD");

    const std::string idStr   = ConsoleUtil::promptInput("User ID           : ");
    if (idStr.empty() || idStr == "b" || idStr == "B") return;
    const std::string newPass = ConsoleUtil::promptPassword("New Temp Password  : ");
    if (newPass.empty()) { ConsoleUtil::printError("Password required."); ConsoleUtil::pause(); return; }

    const auto resp = api.put(
        "/api/admin/users/" + idStr + "/reset-password",
        {{"tempPassword", newPass}},
        AppSession::get().token
    );
    if (!resp.success) ConsoleUtil::printError(resp.errorMessage);
    else               ConsoleUtil::printSuccess("Password reset. User will be prompted to change it on next login.");
    ConsoleUtil::pause();
}


static void deactivateUser(const ApiClient& api) {
    ConsoleUtil::clearScreen();
    ConsoleUtil::printHeader("DEACTIVATE USER");

    const std::string idStr = ConsoleUtil::promptInput("User ID: ");
    if (idStr.empty() || idStr == "b" || idStr == "B") return;

    if (!ConsoleUtil::confirm("Deactivate user " + idStr + "? Their data is preserved.")) {
        ConsoleUtil::printInfo("Cancelled."); ConsoleUtil::pause(); return;
    }

    const auto resp = api.put(
        "/api/admin/users/" + idStr + "/deactivate",
        {},
        AppSession::get().token
    );
    if (!resp.success) ConsoleUtil::printError(resp.errorMessage);
    else               ConsoleUtil::printSuccess("User deactivated.");
    ConsoleUtil::pause();
}


void showAdminUsersMenu(const ApiClient& api) {
    while (true) {
        ConsoleUtil::clearScreen();
        ConsoleUtil::printHeader("MANAGE USERS");

        std::cout << "1. Create User Account\n"
                  << "2. View All Users\n"
                  << "3. Reset User Password\n"
                  << "4. Deactivate User\n"
                  << "5. Back\n"
                  << "\nEnter option: ";

        std::string selectedOption;
        std::getline(std::cin, selectedOption);

        if (selectedOption == "1") createUser(api);
        else if (selectedOption == "2") viewAllUsers(api);
        else if (selectedOption == "3") resetPassword(api);
        else if (selectedOption == "4") deactivateUser(api);
        else if (selectedOption == "5" || selectedOption == "b" || selectedOption == "B") return;
        else {
            ConsoleUtil::printError("Invalid option. Please enter a number from the menu.");
            ConsoleUtil::pause();
        }
    }
}
