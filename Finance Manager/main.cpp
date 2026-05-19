#include "AppFactory.h"
#include "AuthController.h"
#include "MainMenuController.h"

int main() {
    AppFactory factory("finance.db");
    AuthController authController         = factory.createAuthController();
    MainMenuController mainMenuController = factory.createMainMenuController();

    while (true) {
        std::string token = authController.run();
        if (token.empty())
            break;
        mainMenuController.run(token);
        authController.handleLogout(token);
    }
    return 0;
}
