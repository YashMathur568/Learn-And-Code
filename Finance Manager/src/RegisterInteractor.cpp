#include "RegisterInteractor.h"
#include "AuthUtils.h"
#include <stdexcept>

RegisterInteractor::RegisterInteractor(IUserRepository& userRepository)
    : userRepository(userRepository) {}

void RegisterInteractor::execute(const std::string& username, const std::string& password) {
    if (username.empty())
        throw std::runtime_error("Username cannot be empty");
    if (password.size() < 4)
        throw std::runtime_error("Password must be at least 4 characters");
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
