#pragma once

#include "../dto/AdminDtos.hpp"
#include "../models/User.hpp"
#include "../models/Employee.hpp"

#include <optional>
#include <vector>

struct CreatedUserResult {
    User                    user;
    std::optional<Employee> employee;
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
