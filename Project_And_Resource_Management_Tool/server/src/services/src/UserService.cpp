#include "UserService.hpp"
#include "PasswordUtil.hpp"
#include "AppException.hpp"
#include "Resource.hpp"
#include "Designation.hpp"
#include "Department.hpp"
#include "ResourceStatus.hpp"

#include <algorithm>

const std::vector<std::string> UserService::VALID_ROLES = {"ADMIN", "MANAGER", "RESOURCE"};

UserService::UserService(
    std::shared_ptr<IUserRepository>     userRepository,
    std::shared_ptr<IResourceRepository> resourceRepository
)
    : userRepository(std::move(userRepository))
    , resourceRepository(std::move(resourceRepository)) {}

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

    std::string departmentValue;
    std::string designationValue;

    if (request.role == "ADMIN") {

        departmentValue  = Department::ADMINISTRATION;
        designationValue = Designation::ADMINISTRATOR;
    } else if (request.role == "MANAGER") {

        if (request.designation.empty()) {
            throw ValidationException("designation is required for MANAGER role.");
        }
        if (!Designation::isValid(request.designation)) {
            throw ValidationException("Invalid designation value.");
        }
        departmentValue  = Department::MANAGEMENT;
        designationValue = request.designation;
    } else {

        if (request.department.empty() || request.designation.empty()) {
            throw ValidationException("department and designation are required for RESOURCE role.");
        }
        if (!Department::isValid(request.department)) {
            throw ValidationException("Invalid department value.");
        }
        if (!Designation::isValid(request.designation)) {
            throw ValidationException("Invalid designation value.");
        }
        departmentValue  = request.department;
        designationValue = request.designation;
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


    const int newUserId = userRepository->create(newUser);

    const auto createdUser = userRepository->findById(newUserId).value();

    CreatedUserResult result;
    result.user = createdUser;


    Resource userProfile;
    userProfile.userId      = newUserId;
    userProfile.managerId   = 0;
    userProfile.fullName    = request.fullName;
    userProfile.email       = request.email;
    userProfile.department  = departmentValue;
    userProfile.designation = designationValue;

    userProfile.status      = (request.role == "RESOURCE") ? ResourceStatus::BENCH : "";
    resourceRepository->create(userProfile);
    result.resource = resourceRepository->findById(newUserId);

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

