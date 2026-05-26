#include "ValidateTokenInteractor.h"
#include "AuthUtils.h"

ValidateTokenInteractor::ValidateTokenInteractor(ISessionRepository& sessionRepository)
    : sessionRepository(sessionRepository) {}

int ValidateTokenInteractor::execute(const std::string& token) {
    try {
        Session session = sessionRepository.findByToken(token);
        if (isSessionExpired(session.expiresAt))
            return -1;
        return session.userId;
    } catch (...) {
        return -1;
    }
}
