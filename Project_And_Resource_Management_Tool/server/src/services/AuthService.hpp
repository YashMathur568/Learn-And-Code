#pragma once

#include "../dto/AuthDtos.hpp"
#include "../repositories/IUserRepository.hpp"
#include "../repositories/IEmployeeRepository.hpp"

#include <memory>

class AuthService {
public:
    AuthService(
        std::shared_ptr<IUserRepository>     userRepository,
        std::shared_ptr<IEmployeeRepository> employeeRepository
    );

    LoginResponse    login(const LoginRequest& request);
    void             changePassword(int userId, const ChangePasswordRequest& request);

private:
    std::shared_ptr<IUserRepository>     userRepository;
    std::shared_ptr<IEmployeeRepository> employeeRepository;
};
