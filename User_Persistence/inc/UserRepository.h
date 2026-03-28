#ifndef USER_REPOSITORY_H
#define USER_REPOSITORY_H

#include "User.h"

class UserRepository
{
public:
    void save(const User& user) const;
};

#endif
