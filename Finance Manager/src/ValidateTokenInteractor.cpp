#include "ValidateTokenInteractor.h"

ValidateTokenInteractor::ValidateTokenInteractor(ISessionRepository& sessionRepository)
    : sessionRepository(sessionRepository) {}

int ValidateTokenInteractor::execute(const std::string& token) {
    return -1;
}
