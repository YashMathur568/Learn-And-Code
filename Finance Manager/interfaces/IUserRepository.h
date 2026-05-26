#pragma once

#include <string>
#include "User.h"

class IUserRepository {
public:
    virtual ~IUserRepository() = default;
    virtual void save(const User& user) = 0;
    virtual User findByUsername(const std::string& username) = 0;
    virtual bool existsByUsername(const std::string& username) = 0;
};
