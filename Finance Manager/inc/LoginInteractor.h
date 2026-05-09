#pragma once

#include "ILoginInteractor.h"
#include "IUserRepository.h"
#include "ISessionRepository.h"

class LoginInteractor : public ILoginInteractor {
public:
    LoginInteractor(IUserRepository& userRepo, ISessionRepository& sessionRepo);
    std::string execute(const std::string& username, const std::string& password) override;
private:
    IUserRepository& userRepo;
    ISessionRepository& sessionRepo;
};
