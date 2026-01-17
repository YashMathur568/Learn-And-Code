#include "UserService.h"
#include <iostream>

void UserService::saveUser(const User& user) const
{
    if (!validator.isValid(user))
    {
        std::cout << "Invalid user data" << std::endl;
        return;
    }

    repository.save(user);
    backupService.backup(user);
}
