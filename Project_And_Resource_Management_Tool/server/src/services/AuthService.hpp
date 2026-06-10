#pragma once

#include "IAuthService.hpp"
#include "../repositories/IUserRepository.hpp"
#include "../repositories/IEmployeeRepository.hpp"

#include <memory>

class AuthService : public IAuthService {
public:
    AuthService(
        std::shared_ptr<IUserRepository>     userRepository,
        std::shared_ptr<IEmployeeRepository> employeeRepository
    );

    LoginResponse    login(const LoginRequest& request)                               override;
    void             changePassword(int userId, const ChangePasswordRequest& request) override;

private:
    std::shared_ptr<IUserRepository>     userRepository;
    std::shared_ptr<IEmployeeRepository> employeeRepository;
};
