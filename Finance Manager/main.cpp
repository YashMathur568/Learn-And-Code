#include "AppFactory.h"
#include "AuthController.h"
#include "MainMenuController.h"
#include <iostream>

int main() {
    try {
        AppFactory factory("finance.db");
        AuthController& authController         = factory.getAuthController();
        MainMenuController& mainMenuController = factory.getMainMenuController();

        while (true) {
            std::string token = authController.run();
            if (token.empty())
                break;
            mainMenuController.run(token);
            authController.handleLogout(token);
        }
    } catch (const std::exception& exception) {
        std::cerr << "Fatal error: " << exception.what() << std::endl;
        return 1;
    }
    return 0;
}
