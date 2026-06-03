#pragma once

#include "../dto/AuthDtos.hpp"
#include "../repositories/IUserRepository.hpp"

#include <memory>

class AuthService {
public:
    explicit AuthService(std::shared_ptr<IUserRepository> userRepository);

    LoginResponse    login(const LoginRequest& request);
    void             changePassword(int userId, const ChangePasswordRequest& request);

private:
    std::shared_ptr<IUserRepository> userRepository;
};
