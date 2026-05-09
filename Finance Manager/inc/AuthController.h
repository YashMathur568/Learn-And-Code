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
        IInputHandler& input,
        IOutputHandler& output
    );
    void handleRegister();
    std::string handleLogin();           
    void handleLogout(const std::string& token);
private:
    IRegisterInteractor& registerInteractor;
    ILoginInteractor& loginInteractor;
    ILogoutInteractor& logoutInteractor;
    IInputHandler& input;
    IOutputHandler& output;
};
