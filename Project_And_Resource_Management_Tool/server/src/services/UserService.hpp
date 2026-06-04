#pragma once

#include "../dto/AdminDtos.hpp"
#include "../models/User.hpp"
#include "../repositories/IUserRepository.hpp"
#include "../repositories/IEmployeeRepository.hpp"

#include <memory>
#include <vector>

class UserService {
public:
    UserService(
        std::shared_ptr<IUserRepository>     userRepository,
        std::shared_ptr<IEmployeeRepository> employeeRepository
    );

    int               createUser(const CreateUserRequest& request);
    std::vector<User> getAllUsers();
    void              deactivateUser(int userId);
    void              reactivateUser(int userId);
    void              resetPassword(int userId, const ResetPasswordRequest& request);

private:
    std::shared_ptr<IUserRepository>     userRepository;
    std::shared_ptr<IEmployeeRepository> employeeRepository;

    static const std::vector<std::string> VALID_ROLES;

    void validateRole(const std::string& role) const;
};
