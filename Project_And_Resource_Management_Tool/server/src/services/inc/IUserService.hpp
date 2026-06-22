#pragma once

#include "AdminDtos.hpp"
#include "User.hpp"
#include "Resource.hpp"

#include <optional>
#include <vector>

struct CreatedUserResult {
    User                    user;
    std::optional<Resource> resource;
};

class IUserService {
public:
    virtual ~IUserService() = default;

    virtual CreatedUserResult createUser(const CreateUserRequest& request)                   = 0;
    virtual std::vector<User> getAllUsers()                                                   = 0;
    virtual void              deactivateUser(int userId)                                     = 0;
    virtual void              reactivateUser(int userId)                                     = 0;
    virtual void              resetPassword(int userId, const ResetPasswordRequest& request) = 0;
};
