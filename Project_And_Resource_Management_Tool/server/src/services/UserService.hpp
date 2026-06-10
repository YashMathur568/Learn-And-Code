#pragma once

#include "IUserService.hpp"
#include "../repositories/IUserRepository.hpp"
#include "../repositories/IEmployeeRepository.hpp"

#include <memory>
#include <vector>

class UserService : public IUserService {
public:
    UserService(
        std::shared_ptr<IUserRepository>     userRepository,
        std::shared_ptr<IEmployeeRepository> employeeRepository
    );

    CreatedUserResult createUser(const CreateUserRequest& request)                   override;
    std::vector<User> getAllUsers()                                                   override;
    void              deactivateUser(int userId)                                     override;
    void              reactivateUser(int userId)                                     override;
    void              resetPassword(int userId, const ResetPasswordRequest& request) override;

private:
    std::shared_ptr<IUserRepository>     userRepository;
    std::shared_ptr<IEmployeeRepository> employeeRepository;

    static const std::vector<std::string> VALID_ROLES;

    void validateRole(const std::string& role) const;
};
