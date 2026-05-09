#include "LoginInteractor.h"

LoginInteractor::LoginInteractor(IUserRepository& userRepo, ISessionRepository& sessionRepo)
    : userRepo(userRepo), sessionRepo(sessionRepo) {}

std::string LoginInteractor::execute(const std::string& username, const std::string& password) {
    
    return "";
}
