#include "AuthController.h"

AuthController::AuthController(
    IRegisterInteractor& registerInteractor,
    ILoginInteractor& loginInteractor,
    ILogoutInteractor& logoutInteractor,
    IInputHandler& input,
    IOutputHandler& output)
    : registerInteractor(registerInteractor), loginInteractor(loginInteractor),
      logoutInteractor(logoutInteractor), input(input), output(output) {}

void AuthController::handleRegister()                        {  }
std::string AuthController::handleLogin()                    {  return ""; }
void AuthController::handleLogout(const std::string& token)  {  }
