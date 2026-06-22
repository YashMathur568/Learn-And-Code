#include "LoginScreen.hpp"
#include "AppSession.hpp"
#include "ConsoleUtil.hpp"

#include <iostream>


static bool showChangePasswordScreen(const ApiClient& api, const std::string& oldPassword);


bool showLoginScreen(const ApiClient& api) {
    ConsoleUtil::clearScreen();
    ConsoleUtil::printHeader(
        "PROJECT & RESOURCE MANAGEMENT TOOL",
        "Learn & Code — Final Project"
    );

    const std::string username = ConsoleUtil::promptInput("Username : ");
    if (username.empty()) return false;

    const std::string password = ConsoleUtil::promptPassword("Password : ");
    if (password.empty()) return false;

    std::cout << "\nLogging in...\n";

    const auto resp = api.post("/api/auth/login", {
        {"username", username},
        {"password", password}
    });

    if (!resp.success) {
        ConsoleUtil::printError(resp.errorMessage);
        ConsoleUtil::pause();
        return false;
    }

    auto& session              = AppSession::get();
    session.token              = resp.body.value("token", "");
    session.loggedIn           = true;
    session.forcePasswordChange = resp.body.value("forcePasswordChange", false);

    const auto& user           = resp.body.contains("user") ? resp.body["user"] : resp.body;
    session.userId             = user.value("userId", 0);
    session.username           = user.value("username", username);
    session.fullName           = user.value("fullName", username);
    session.role               = user.value("role", "");

    if (session.forcePasswordChange) {
        return showChangePasswordScreen(api, password);
    }

    return true;
}


static bool showChangePasswordScreen(const ApiClient& api, const std::string& oldPassword) {
    while (true) {
        ConsoleUtil::clearScreen();
        ConsoleUtil::printHeader(
            "CHANGE PASSWORD",
            "You must set allocation new password to continue."
        );

        const std::string newPass     = ConsoleUtil::promptPassword("New Password     : ");
        const std::string confirmPass = ConsoleUtil::promptPassword("Confirm Password : ");

        if (newPass.empty()) {
            ConsoleUtil::printError("Password cannot be empty.");
            ConsoleUtil::pause();
            continue;
        }
        if (newPass == oldPassword) {
            ConsoleUtil::printError("New password must be different from your current password.");
            ConsoleUtil::pause();
            continue;
        }
        if (newPass != confirmPass) {
            ConsoleUtil::printError("Passwords do not match.");
            ConsoleUtil::pause();
            continue;
        }

        std::cout << "\nSaving...\n";

        const auto resp = api.post(
            "/api/auth/change-password",
            {{"currentPassword", oldPassword}, {"newPassword", newPass}, {"confirmPassword", confirmPass}},
            AppSession::get().token
        );

        if (!resp.success) {
            ConsoleUtil::printError(resp.errorMessage);
            ConsoleUtil::pause();
            continue;
        }

        AppSession::get().forcePasswordChange = false;
        ConsoleUtil::printSuccess("Password updated. Welcome!");
        ConsoleUtil::pause();
        return true;
    }
}
