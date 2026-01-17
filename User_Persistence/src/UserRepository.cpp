#include "UserRepository.h"
#include <iostream>

void UserRepository::save(const User& user) const
{
    std::cout << "Saving user to database: " << user.id << std::endl;
}
