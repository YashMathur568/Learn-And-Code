#include "RegisterInteractor.h"

RegisterInteractor::RegisterInteractor(IUserRepository& userRepo)
    : userRepo(userRepo) {}

void RegisterInteractor::execute(const std::string& username, const std::string& password) {
    
}
