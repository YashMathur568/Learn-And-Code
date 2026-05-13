#include "RegisterInteractor.h"

RegisterInteractor::RegisterInteractor(IUserRepository& userRepository)
    : userRepository(userRepository) {}

void RegisterInteractor::execute(const std::string& username, const std::string& password) {
}
