#include "AuthService.hpp"
#include "../security/JwtUtil.hpp"
#include "../security/PasswordUtil.hpp"
#include "../utils/AppException.hpp"

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

    // password_expires_at <= NOW()  =>  force change
    bool forceChange = false;
    if (!user.passwordExpiresAt.empty()) {
        // Parse "YYYY-MM-DD HH:MM:SS" into time_t for comparison
        std::tm tm{};
        int year, month, day, hour, minute, second;
        if (std::sscanf(user.passwordExpiresAt.c_str(), "%d-%d-%d %d:%d:%d",
                        &year, &month, &day, &hour, &minute, &second) == 6) {
            tm.tm_year  = year - 1900;
            tm.tm_mon   = month - 1;
            tm.tm_mday  = day;
            tm.tm_hour  = hour;
            tm.tm_min   = minute;
            tm.tm_sec   = second;
            tm.tm_isdst = -1;
            std::time_t expiry = std::mktime(&tm);
            forceChange = (expiry <= std::time(nullptr));
        } else {
            forceChange = true; // unparseable → treat as expired
        }
    } else {
        forceChange = true; // null → treat as expired
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

    // When password is expired the user proved identity at login; skip current-password check.
    bool isExpired = false;
    if (!user.passwordExpiresAt.empty()) {
        std::tm tm{};
        int year, month, day, hour, minute, second;
        if (std::sscanf(user.passwordExpiresAt.c_str(), "%d-%d-%d %d:%d:%d",
                        &year, &month, &day, &hour, &minute, &second) == 6) {
            tm.tm_year  = year - 1900;
            tm.tm_mon   = month - 1;
            tm.tm_mday  = day;
            tm.tm_hour  = hour;
            tm.tm_min   = minute;
            tm.tm_sec   = second;
            tm.tm_isdst = -1;
            std::time_t expiry = std::mktime(&tm);
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
