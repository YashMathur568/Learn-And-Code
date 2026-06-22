#include "UserController.hpp"
#include "UserService.hpp"
#include "MySQLUserRepository.hpp"
#include "MySQLResourceRepository.hpp"
#include "RoleGuard.hpp"
#include "PermissionGuard.hpp"
#include "AppException.hpp"
#include "ResponseBuilder.hpp"

#include <nlohmann/json.hpp>

UserController::UserController()
    : userService(std::make_shared<UserService>(
          std::make_shared<MySQLUserRepository>(),
          std::make_shared<MySQLResourceRepository>()
      )) {}

void UserController::getAllUsers(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback
) {
    try {
        PermissionGuard::require(request, "MANAGE_USERS");

        const auto users = userService->getAllUsers();
        nlohmann::json dataArray = nlohmann::json::array();
        for (const auto& user : users) {
            dataArray.push_back(userToJson(user));
        }
        callback(ResponseBuilder::success({{"data", dataArray}}));

    } catch (const UnauthorizedException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k403Forbidden));
    } catch (const AppException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k500InternalServerError));
    }
}

void UserController::createUser(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback
) {
    try {
        PermissionGuard::require(request, "MANAGE_USERS");

        const auto jsonBody     = nlohmann::json::parse(request->getBody());
        const auto createRequest = CreateUserRequest::fromJson(jsonBody);

        const CreatedUserResult result = userService->createUser(createRequest);

        nlohmann::json responseData = userToJson(result.user);
        if (result.resource.has_value()) {
            responseData["resource"] = resourceToJson(result.resource.value());
        }
        callback(ResponseBuilder::success(responseData, drogon::k201Created));

    } catch (const UnauthorizedException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k403Forbidden));
    } catch (const ConflictException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k409Conflict));
    } catch (const ValidationException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k400BadRequest));
    } catch (const nlohmann::json::exception& exception) {
        callback(ResponseBuilder::error(std::string("Invalid JSON: ") + exception.what(), drogon::k400BadRequest));
    } catch (const AppException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k500InternalServerError));
    }
}

void UserController::deactivateUser(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback,
    int targetUserId
) {
    try {
        PermissionGuard::require(request, "MANAGE_USERS");

        const auto claims = RoleGuard::extractClaims(request);
        if (claims.userId == targetUserId) {
            callback(ResponseBuilder::error("Admin cannot deactivate their own account.", drogon::k400BadRequest));
            return;
        }

        userService->deactivateUser(targetUserId);
        callback(ResponseBuilder::success({{"message", "User deactivated successfully."}}));

    } catch (const UnauthorizedException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k403Forbidden));
    } catch (const NotFoundException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k404NotFound));
    } catch (const ValidationException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k400BadRequest));
    } catch (const AppException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k500InternalServerError));
    }
}

void UserController::reactivateUser(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback,
    int targetUserId
) {
    try {
        PermissionGuard::require(request, "MANAGE_USERS");

        userService->reactivateUser(targetUserId);
        callback(ResponseBuilder::success({{"message", "User reactivated successfully."}}));

    } catch (const UnauthorizedException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k403Forbidden));
    } catch (const NotFoundException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k404NotFound));
    } catch (const ValidationException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k400BadRequest));
    } catch (const AppException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k500InternalServerError));
    }
}

void UserController::resetPassword(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback,
    int targetUserId
) {
    try {
        PermissionGuard::require(request, "MANAGE_USERS");

        const auto jsonBody      = nlohmann::json::parse(request->getBody());
        const auto resetRequest  = ResetPasswordRequest::fromJson(jsonBody);

        userService->resetPassword(targetUserId, resetRequest);
        callback(ResponseBuilder::success({{"message", "Password reset successfully."}}));

    } catch (const UnauthorizedException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k403Forbidden));
    } catch (const NotFoundException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k404NotFound));
    } catch (const ValidationException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k400BadRequest));
    } catch (const nlohmann::json::exception& exception) {
        callback(ResponseBuilder::error(std::string("Invalid JSON: ") + exception.what(), drogon::k400BadRequest));
    } catch (const AppException& exception) {
        callback(ResponseBuilder::error(exception.what(), drogon::k500InternalServerError));
    }
}
