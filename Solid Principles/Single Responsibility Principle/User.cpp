#include "User.h"

User::User(const std::string& name, int age)
    : userName(name),
      userAge(age)
{
}

std::string User::getName() const
{
    return userName;
}

int User::getAge() const
{
    return userAge;
}
