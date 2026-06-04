#pragma once

#include "../models/User.hpp"
#include <optional>
#include <string>
#include <vector>

class IUserRepository {
public:
    virtual ~IUserRepository() = default;

    virtual std::optional<User> findByUsername(const std::string& username)    = 0;
    virtual std::optional<User> findById(int userId)                           = 0;
    virtual std::vector<User>   findAll()                                      = 0;
    virtual int                 create(const User& user)                       = 0;
    virtual void                updatePasswordHash(int userId,
                                                   const std::string& newHash) = 0;
    virtual void                setForcePwdChange(int userId, bool value)      = 0;
    virtual void                setActiveStatus(int userId, bool active)       = 0;
    virtual bool                existsByUsername(const std::string& username)  = 0;
    virtual bool                existsByEmail(const std::string& email)        = 0;
};
