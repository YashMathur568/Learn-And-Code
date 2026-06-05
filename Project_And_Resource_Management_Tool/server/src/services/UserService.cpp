#include "UserService.hpp"
#include "../security/PasswordUtil.hpp"
#include "../utils/AppException.hpp"
#include "../models/Employee.hpp"

#include <algorithm>

const std::vector<std::string> UserService::VALID_ROLES = {"ADMIN", "MANAGER", "EMPLOYEE"};

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
        throw ValidationException("Invalid role. Must be ADMIN, MANAGER, or EMPLOYEE.");
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
            throw ValidationException("department and designation are required for EMPLOYEE and MANAGER roles.");
        }
    }

    if (userRepository->existsByUsername(request.username)) {
        throw ConflictException("Username '" + request.username + "' is already taken.");
    }

    if (userRepository->existsByEmail(request.email)) {
        throw ConflictException("Email '" + request.email + "' is already registered.");
    }

    User newUser;
    newUser.fullName       = request.fullName;
    newUser.email          = request.email;
    newUser.username       = request.username;
    newUser.passwordHash   = PasswordUtil::hash(request.tempPassword);
    newUser.role           = request.role;
    newUser.isActive       = true;
    newUser.forcePwdChange = true;

    const int newUserId = userRepository->create(newUser);

    const auto createdUser = userRepository->findById(newUserId).value();

    CreatedUserResult result;
    result.user = createdUser;

    if (request.role != "ADMIN") {
        Employee employeeProfile;
        employeeProfile.userId      = newUserId;
        employeeProfile.fullName    = request.fullName;
        employeeProfile.email       = request.email;
        employeeProfile.department  = request.department;
        employeeProfile.designation = request.designation;
        employeeRepository->create(employeeProfile);
        result.employee = employeeRepository->findByUserId(newUserId);
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

    const std::string newHash = PasswordUtil::hash(request.tempPassword);
    userRepository->updatePasswordHash(userId, newHash);
    userRepository->setForcePwdChange(userId, true);
}
