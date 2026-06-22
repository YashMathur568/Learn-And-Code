#include "AuthService.hpp"
#include "JwtUtil.hpp"
#include "PasswordUtil.hpp"
#include "AppException.hpp"

#include <ctime>
#include <memory>

AuthService::AuthService(
    std::shared_ptr<IUserRepository> userRepository
)
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

    const std::string token = JwtUtil::generate(user.userId, user.role);

    bool forceChange = false;
    if (!user.passwordExpiresAt.empty()) {
        std::tm expiryTimeStructure{};
        int year, month, day, hour, minute, second;
        if (std::sscanf(user.passwordExpiresAt.c_str(), "%d-%d-%d %d:%d:%d",
                        &year, &month, &day, &hour, &minute, &second) == 6) {
            expiryTimeStructure.tm_year  = year - 1900;
            expiryTimeStructure.tm_mon   = month - 1;
            expiryTimeStructure.tm_mday  = day;
            expiryTimeStructure.tm_hour  = hour;
            expiryTimeStructure.tm_min   = minute;
            expiryTimeStructure.tm_sec   = second;
            expiryTimeStructure.tm_isdst = -1;
            std::time_t expiry = std::mktime(&expiryTimeStructure);
            forceChange = (expiry <= std::time(nullptr));
        } else {
            forceChange = true;
        }
    } else {
        forceChange = true;
    }

    LoginResponse response;
    response.token              = token;
    response.role               = user.role;
    response.forcePasswordChange = forceChange;
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

    bool isExpired = false;
    if (!user.passwordExpiresAt.empty()) {
        std::tm expiryTimeStructure{};
        int year, month, day, hour, minute, second;
        if (std::sscanf(user.passwordExpiresAt.c_str(), "%d-%d-%d %d:%d:%d",
                        &year, &month, &day, &hour, &minute, &second) == 6) {
            expiryTimeStructure.tm_year  = year - 1900;
            expiryTimeStructure.tm_mon   = month - 1;
            expiryTimeStructure.tm_mday  = day;
            expiryTimeStructure.tm_hour  = hour;
            expiryTimeStructure.tm_min   = minute;
            expiryTimeStructure.tm_sec   = second;
            expiryTimeStructure.tm_isdst = -1;
            std::time_t expiry = std::mktime(&expiryTimeStructure);
            isExpired = (expiry <= std::time(nullptr));
        } else {
            isExpired = true;
        }
    } else {
        isExpired = true;
    }

    if (!isExpired) {
        if (!PasswordUtil::verify(request.currentPassword, user.passwordHash)) {
            throw UnauthorizedException("Current password is incorrect.");
        }
    }

    const std::string newHash = PasswordUtil::hash(request.newPassword);
    userRepository->updatePasswordHash(userId, newHash);
    userRepository->refreshPasswordExpiry(userId);
}
