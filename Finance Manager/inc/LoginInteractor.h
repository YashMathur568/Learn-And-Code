#pragma once

#include "ILoginInteractor.h"
#include "IUserRepository.h"
#include "ISessionRepository.h"

class LoginInteractor : public ILoginInteractor {
public:
    LoginInteractor(IUserRepository& userRepository, ISessionRepository& sessionRepository);
    std::string execute(const std::string& username, const std::string& password) override;
private:
    IUserRepository& userRepository;
    ISessionRepository& sessionRepository;
};
