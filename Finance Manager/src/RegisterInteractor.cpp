#include "RegisterInteractor.h"
#include "AuthUtils.h"
#include <stdexcept>

RegisterInteractor::RegisterInteractor(IUserRepository& userRepository)
    : userRepository(userRepository) {}

void RegisterInteractor::execute(const std::string& username, const std::string& password) {
    if (userRepository.existsByUsername(username))
        throw std::runtime_error("Username already taken");
    std::string salt         = generateSalt();
    std::string passwordHash = hashPassword(password, salt);
    User newUser;
    newUser.username     = username;
    newUser.passwordHash = passwordHash;
    newUser.salt         = salt;
    newUser.createdAt    = getCurrentTimestamp();
    userRepository.save(newUser);
}
