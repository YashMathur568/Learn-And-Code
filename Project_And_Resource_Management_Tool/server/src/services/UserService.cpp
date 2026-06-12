#include "UserService.hpp"
#include "../security/PasswordUtil.hpp"
#include "../utils/AppException.hpp"
#include "../models/Employee.hpp"

#include <algorithm>

const std::vector<std::string> UserService::VALID_ROLES = {"ADMIN", "MANAGER", "RESOURCE"};

UserService::UserService(
    std::shared_ptr<IUserRepository>     userRepository,
    std::shared_ptr<IEmployeeRepository> employeeRepository
)
    : userRepository(std::move(userRepository))
    , employeeRepository(std::move(employeeRepository)) {}

void UserService::validateRole(const std::string& role) const {
    bool roleIsValid = std::any_of(
        VALID_ROLES.begin(),
        VALID_ROLES.end(),
        [&role](const std::string& validRole) { return validRole == role; }
    );
    if (!roleIsValid) {
        throw ValidationException("Invalid role. Must be ADMIN, MANAGER, or RESOURCE.");
    }
}

CreatedUserResult UserService::createUser(const CreateUserRequest& request) {
    if (request.fullName.empty() || request.email.empty() ||
        request.username.empty() || request.tempPassword.empty()) {
        throw ValidationException("fullName, email, username, and tempPassword are required.");
    }

    validateRole(request.role);

    if (request.role != "ADMIN") {
        if (request.department.empty() || request.designation.empty()) {
            throw ValidationException("department and designation are required for MANAGER and RESOURCE roles.");
        }
    }

    if (userRepository->existsByUsername(request.username)) {
        throw ConflictException("Username '" + request.username + "' is already taken.");
    }

    if (userRepository->existsByEmail(request.email)) {
        throw ConflictException("Email '" + request.email + "' is already registered.");
    }

    User newUser;
    newUser.fullName     = request.fullName;
    newUser.email        = request.email;
    newUser.username     = request.username;
    if (!PasswordUtil::meetsStrengthPolicy(request.tempPassword)) {
        throw ValidationException("Temporary password does not meet strength requirements (min 8 chars, uppercase, digit, special character).");
    }
    newUser.passwordHash = PasswordUtil::hash(request.tempPassword);
    newUser.role         = request.role;
    newUser.isActive     = true;
    // password_expires_at defaults to NOW() via DB — force change on first login

    const int newUserId = userRepository->create(newUser);

    const auto createdUser = userRepository->findById(newUserId).value();

    CreatedUserResult result;
    result.user = createdUser;

    if (request.role == "MANAGER" || request.role == "RESOURCE") {
        Employee employeeProfile;
        employeeProfile.userId      = newUserId;
        employeeProfile.managerId   = 0; // assigned later via assign-manager
        employeeProfile.fullName    = request.fullName;
        employeeProfile.email       = request.email;
        employeeProfile.department  = request.department;
        employeeProfile.designation = request.designation;
        // Non-empty status signals RESOURCE so create() also inserts resource_status
        employeeProfile.status      = (request.role == "RESOURCE") ? "BENCH" : "";
        employeeRepository->create(employeeProfile);
        result.employee = employeeRepository->findById(newUserId);
    }

    return result;
}

std::vector<User> UserService::getAllUsers() {
    return userRepository->findAll();
}

void UserService::deactivateUser(int userId) {
    auto optionalUser = userRepository->findById(userId);
    if (!optionalUser.has_value()) {
        throw NotFoundException("User with ID " + std::to_string(userId) + " not found.");
    }
    if (!optionalUser->isActive) {
        throw ValidationException("User is already inactive.");
    }
    userRepository->setActiveStatus(userId, false);
}

void UserService::reactivateUser(int userId) {
    auto optionalUser = userRepository->findById(userId);
    if (!optionalUser.has_value()) {
        throw NotFoundException("User with ID " + std::to_string(userId) + " not found.");
    }
    if (optionalUser->isActive) {
        throw ValidationException("User is already active.");
    }
    userRepository->setActiveStatus(userId, true);
}

void UserService::resetPassword(int userId, const ResetPasswordRequest& request) {
    auto optionalUser = userRepository->findById(userId);
    if (!optionalUser.has_value()) {
        throw NotFoundException("User with ID " + std::to_string(userId) + " not found.");
    }

    if (!PasswordUtil::meetsStrengthPolicy(request.tempPassword)) {
        throw ValidationException("Temporary password does not meet strength requirements (min 8 chars, uppercase, digit, special character).");
    }
    const std::string newHash = PasswordUtil::hash(request.tempPassword);
    userRepository->updatePasswordHash(userId, newHash);
    userRepository->expirePasswordNow(userId);
}

