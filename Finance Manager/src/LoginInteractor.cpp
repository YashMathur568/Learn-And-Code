#include "LoginInteractor.h"

LoginInteractor::LoginInteractor(IUserRepository& userRepository, ISessionRepository& sessionRepository)
    : userRepository(userRepository), sessionRepository(sessionRepository) {}

std::string LoginInteractor::execute(const std::string& username, const std::string& password) {
    return "";
}
