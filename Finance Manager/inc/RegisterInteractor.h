#pragma once

#include "IRegisterInteractor.h"
#include "IUserRepository.h"

class RegisterInteractor : public IRegisterInteractor {
public:
    explicit RegisterInteractor(IUserRepository& userRepository);
    void execute(const std::string& username, const std::string& password) override;
private:
    IUserRepository& userRepository;
};
