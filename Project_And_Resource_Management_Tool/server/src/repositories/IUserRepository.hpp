#pragma once

#include "../models/User.hpp"
#include <optional>
#include <string>

class IUserRepository {
public:
    virtual ~IUserRepository() = default;

    virtual std::optional<User> findByUsername(const std::string& username)    = 0;
    virtual std::optional<User> findById(int userId)                           = 0;
    virtual void                updatePasswordHash(int userId,
                                                   const std::string& newHash) = 0;
    virtual void                setForcePwdChange(int userId, bool value)      = 0;
};
