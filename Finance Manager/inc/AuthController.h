#pragma once

#include <string>
#include "IRegisterInteractor.h"
#include "ILoginInteractor.h"
#include "ILogoutInteractor.h"
#include "IInputHandler.h"
#include "IOutputHandler.h"

class AuthController {
public:
    AuthController(
        IRegisterInteractor& registerInteractor,
        ILoginInteractor& loginInteractor,
        ILogoutInteractor& logoutInteractor,
        IInputHandler& inputHandler,
        IOutputHandler& outputHandler
    );
    void handleRegister();
    std::string handleLogin();
    void handleLogout(const std::string& token);
    std::string run();
private:
    IRegisterInteractor& registerInteractor;
    ILoginInteractor& loginInteractor;
    ILogoutInteractor& logoutInteractor;
    IInputHandler& inputHandler;
    IOutputHandler& outputHandler;
};
