#pragma once

#include "ILogoutInteractor.h"
#include "ISessionRepository.h"

class LogoutInteractor : public ILogoutInteractor {
public:
    explicit LogoutInteractor(ISessionRepository& sessionRepo);
    void execute(const std::string& token) override;
private:
    ISessionRepository& sessionRepo;
};
