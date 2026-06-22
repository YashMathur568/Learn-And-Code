#pragma once

#include "AuthDtos.hpp"

class IAuthService {
public:
    virtual ~IAuthService() = default;

    virtual LoginResponse login(const LoginRequest& request)                               = 0;
    virtual void          changePassword(int userId, const ChangePasswordRequest& request) = 0;
};
