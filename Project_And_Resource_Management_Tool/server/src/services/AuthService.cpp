#include "AuthService.hpp"
#include "../security/JwtUtil.hpp"
#include "../security/PasswordUtil.hpp"
#include "../repositories/MySQLUserRepository.hpp"
#include "../utils/AppException.hpp"

#include <memory>

AuthService::AuthService(std::shared_ptr<IUserRepository> userRepository)
    : userRepository(std::move(userRepository)) {}

LoginResponse AuthService::login(const LoginRequest& request) {
    auto optionalUser = userRepository->findByUsername(request.username);

    if (!optionalUser.has_value()) {
        throw UnauthorizedException("Invalid username or password.");
    }

    const User& user = optionalUser.value();

    if (!user.isActive) {
        throw UnauthorizedException("Account is inactive. Contact your administrator.");
    }

    if (!PasswordUtil::verify(request.password, user.passwordHash)) {
        throw UnauthorizedException("Invalid username or password.");
    }

    int resolvedEmployeeId = 0;

    const std::string token = JwtUtil::generate(user.userId, user.role, resolvedEmployeeId);

    LoginResponse response;
    response.token              = token;
    response.role               = user.role;
    response.forcePasswordChange = user.forcePwdChange;
    return response;
}

void AuthService::changePassword(int userId, const ChangePasswordRequest& request) {
    if (request.newPassword != request.confirmPassword) {
        throw ValidationException("New password and confirm password do not match.");
    }

    if (!PasswordUtil::meetsStrengthPolicy(request.newPassword)) {
        throw ValidationException(
            "Password must be at least 8 characters and include "
            "one uppercase letter, one digit, and one special character."
        );
    }

    auto optionalUser = userRepository->findById(userId);
    if (!optionalUser.has_value()) {
        throw NotFoundException("User not found.");
    }

    const User& user = optionalUser.value();

    if (!PasswordUtil::verify(request.currentPassword, user.passwordHash)) {
        throw UnauthorizedException("Current password is incorrect.");
    }

    const std::string newHash = PasswordUtil::hash(request.newPassword);
    userRepository->updatePasswordHash(userId, newHash);
    userRepository->setForcePwdChange(userId, false);
}
