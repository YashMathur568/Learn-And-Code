#pragma once

#include "IAuthService.hpp"
#include "IUserRepository.hpp"

#include <memory>

class AuthService : public IAuthService {
public:
    explicit AuthService(std::shared_ptr<IUserRepository> userRepository);

    LoginResponse    login(const LoginRequest& request)                               override;
    void             changePassword(int userId, const ChangePasswordRequest& request) override;

private:
    std::shared_ptr<IUserRepository> userRepository;
};
