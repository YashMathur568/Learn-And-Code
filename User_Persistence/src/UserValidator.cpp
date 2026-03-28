#include "UserValidator.h"

bool UserValidator::isValid(const User& user) const
{
    return !user.name.empty() && !user.email.empty();
}
