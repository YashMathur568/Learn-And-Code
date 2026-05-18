#include "LoginInteractor.h"
#include "AuthUtils.h"
#include <stdexcept>

LoginInteractor::LoginInteractor(IUserRepository& userRepository, ISessionRepository& sessionRepository)
    : userRepository(userRepository), sessionRepository(sessionRepository) {}

std::string LoginInteractor::execute(const std::string& username, const std::string& password) {
    User user = userRepository.findByUsername(username);
    std::string computedHash = hashPassword(password, user.salt);
    if (computedHash != user.passwordHash)
        throw std::runtime_error("Invalid credentials");
    std::string token = generateSessionToken();
    Session newSession;
    newSession.userId    = user.id;
    newSession.token     = token;
    newSession.expiresAt = getExpiryTimestamp(24);
    sessionRepository.save(newSession);
    return token;
}
