#include "AuthController.h"

AuthController::AuthController(
    IRegisterInteractor& registerInteractor,
    ILoginInteractor& loginInteractor,
    ILogoutInteractor& logoutInteractor,
    IInputHandler& inputHandler,
    IOutputHandler& outputHandler)
    : registerInteractor(registerInteractor), loginInteractor(loginInteractor),
      logoutInteractor(logoutInteractor), inputHandler(inputHandler), outputHandler(outputHandler) {}

void AuthController::handleRegister()                        {}
std::string AuthController::handleLogin()                    { return ""; }
void AuthController::handleLogout(const std::string& token)  {}
