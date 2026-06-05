#include "UserController.hpp"
#include "../repositories/MySQLUserRepository.hpp"
#include "../repositories/MySQLEmployeeRepository.hpp"
#include "../security/RoleGuard.hpp"
#include "../utils/AppException.hpp"
#include "../utils/ResponseBuilder.hpp"

#include <nlohmann/json.hpp>

UserController::UserController()
    : userService(std::make_shared<UserService>(
          std::make_shared<MySQLUserRepository>(),
          std::make_shared<MySQLEmployeeRepository>()
      )) {}

void UserController::getAllUsers(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback
) {
    try {
        RoleGuard::requireRole(request, "ADMIN");

        const auto users = userService->getAllUsers();
        nlohmann::json dataArray = nlohmann::json::array();
        for (const auto& user : users) {
            dataArray.push_back(userToJson(user));
        }
        callback(ResponseBuilder::success({{"data", dataArray}}));

    } catch (const UnauthorizedException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k403Forbidden));
    } catch (const AppException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k500InternalServerError));
    }
}

void UserController::createUser(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback
) {
    try {
        RoleGuard::requireRole(request, "ADMIN");

        const auto jsonBody     = nlohmann::json::parse(request->getBody());
        const auto createRequest = CreateUserRequest::fromJson(jsonBody);

        const CreatedUserResult result = userService->createUser(createRequest);

        nlohmann::json responseData = userToJson(result.user);
        if (result.employee.has_value()) {
            responseData["employee"] = employeeToJson(result.employee.value());
        }
        callback(ResponseBuilder::success(responseData, drogon::k201Created));

    } catch (const UnauthorizedException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k403Forbidden));
    } catch (const ConflictException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k409Conflict));
    } catch (const ValidationException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k400BadRequest));
    } catch (const nlohmann::json::exception& ex) {
        callback(ResponseBuilder::error(std::string("Invalid JSON: ") + ex.what(), drogon::k400BadRequest));
    } catch (const AppException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k500InternalServerError));
    }
}

void UserController::deactivateUser(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback,
    int targetUserId
) {
    try {
        RoleGuard::requireRole(request, "ADMIN");

        const auto claims = RoleGuard::extractClaims(request);
        if (claims.userId == targetUserId) {
            callback(ResponseBuilder::error("Admin cannot deactivate their own account.", drogon::k400BadRequest));
            return;
        }

        userService->deactivateUser(targetUserId);
        callback(ResponseBuilder::success({{"message", "User deactivated successfully."}}));

    } catch (const UnauthorizedException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k403Forbidden));
    } catch (const NotFoundException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k404NotFound));
    } catch (const ValidationException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k400BadRequest));
    } catch (const AppException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k500InternalServerError));
    }
}

void UserController::reactivateUser(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback,
    int targetUserId
) {
    try {
        RoleGuard::requireRole(request, "ADMIN");

        userService->reactivateUser(targetUserId);
        callback(ResponseBuilder::success({{"message", "User reactivated successfully."}}));

    } catch (const UnauthorizedException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k403Forbidden));
    } catch (const NotFoundException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k404NotFound));
    } catch (const ValidationException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k400BadRequest));
    } catch (const AppException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k500InternalServerError));
    }
}

void UserController::resetPassword(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback,
    int targetUserId
) {
    try {
        RoleGuard::requireRole(request, "ADMIN");

        const auto jsonBody      = nlohmann::json::parse(request->getBody());
        const auto resetRequest  = ResetPasswordRequest::fromJson(jsonBody);

        userService->resetPassword(targetUserId, resetRequest);
        callback(ResponseBuilder::success({{"message", "Password reset successfully."}}));

    } catch (const UnauthorizedException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k403Forbidden));
    } catch (const NotFoundException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k404NotFound));
    } catch (const ValidationException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k400BadRequest));
    } catch (const nlohmann::json::exception& ex) {
        callback(ResponseBuilder::error(std::string("Invalid JSON: ") + ex.what(), drogon::k400BadRequest));
    } catch (const AppException& ex) {
        callback(ResponseBuilder::error(ex.what(), drogon::k500InternalServerError));
    }
}
