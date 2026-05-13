#include "LogoutInteractor.h"

LogoutInteractor::LogoutInteractor(ISessionRepository& sessionRepository)
    : sessionRepository(sessionRepository) {}

void LogoutInteractor::execute(const std::string& token) {
}
