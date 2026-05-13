#pragma once

#include "IValidateTokenInteractor.h"
#include "ISessionRepository.h"

class ValidateTokenInteractor : public IValidateTokenInteractor {
public:
    explicit ValidateTokenInteractor(ISessionRepository& sessionRepository);
    int execute(const std::string& token) override;
private:
    ISessionRepository& sessionRepository;
};
