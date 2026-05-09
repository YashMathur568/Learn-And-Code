#include "LogoutInteractor.h"

LogoutInteractor::LogoutInteractor(ISessionRepository& sessionRepo)
    : sessionRepo(sessionRepo) {}

void LogoutInteractor::execute(const std::string& token) {
    
}
