#include "AuthController.h"
#include <stdexcept>

AuthController::AuthController(
    IRegisterInteractor& registerInteractor,
    ILoginInteractor& loginInteractor,
    ILogoutInteractor& logoutInteractor,
    IInputHandler& inputHandler,
    IOutputHandler& outputHandler)
    : registerInteractor(registerInteractor), loginInteractor(loginInteractor),
      logoutInteractor(logoutInteractor), inputHandler(inputHandler), outputHandler(outputHandler) {}

void AuthController::handleRegister() {
    outputHandler.print("\n--- Register ---");
    std::string username = inputHandler.readString("Username: ");
    std::string password = inputHandler.readPassword("Password: ");
    try {
        registerInteractor.execute(username, password);
        outputHandler.print("Registration successful. You can now log in.");
    } catch (const std::exception& exception) {
        outputHandler.printError(exception.what());
    }
}

std::string AuthController::handleLogin() {
    outputHandler.print("\n--- Login ---");
    std::string username = inputHandler.readString("Username: ");
    std::string password = inputHandler.readPassword("Password: ");
    try {
        std::string token = loginInteractor.execute(username, password);
        outputHandler.print("Login successful. Welcome, " + username + "!");
        return token;
    } catch (const std::exception& exception) {
        outputHandler.printError(exception.what());
        return "";
    }
}

void AuthController::handleLogout(const std::string& token) {
    try {
        logoutInteractor.execute(token);
        outputHandler.print("Logged out successfully.");
    } catch (const std::exception& exception) {
        outputHandler.printError(exception.what());
    }
}

std::string AuthController::run() {
    while (true) {
        outputHandler.print("\n=== Personal Finance Manager ===");
        outputHandler.print("1. Register");
        outputHandler.print("2. Login");
        outputHandler.print("3. Exit");
        int choice = inputHandler.readInt("Select option: ");
        switch (choice) {
            case 1:
                handleRegister();
                break;
            case 2: {
                std::string token = handleLogin();
                if (!token.empty())
                    return token;
                break;
            }
            case 3:
                return "";
            default:
                outputHandler.printError("Invalid option.");
                break;
        }
    }
}
