#pragma once

#include "IValidateTokenInteractor.h"
#include "ISessionRepository.h"

class ValidateTokenInteractor : public IValidateTokenInteractor {
public:
    explicit ValidateTokenInteractor(ISessionRepository& sessionRepo);
    
    int execute(const std::string& token) override;
private:
    ISessionRepository& sessionRepo;
};
