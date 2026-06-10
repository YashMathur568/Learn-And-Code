#include "AuthController.hpp"
#include "../services/AuthService.hpp"
#include "../repositories/MySQLUserRepository.hpp"
#include "../repositories/MySQLEmployeeRepository.hpp"
#include "../security/RoleGuard.hpp"
#include "../security/TokenBlacklist.hpp"
#include "../utils/AppException.hpp"
#include "../utils/ResponseBuilder.hpp"
#include "../dto/AuthDtos.hpp"

#include <nlohmann/json.hpp>

AuthController::AuthController()
    : authService(std::make_shared<AuthService>(
          std::make_shared<MySQLUserRepository>(),
          std::make_shared<MySQLEmployeeRepository>()
      )) {}

void AuthController::login(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback
) {
    try {
        const auto jsonBody = nlohmann::json::parse(request->getBody());
        const auto loginRequest = LoginRequest::fromJson(jsonBody);

        LoginResponse loginResponse = authService->login(loginRequest);

        callback(ResponseBuilder::success(loginResponse.toJson()));

    } catch (const UnauthorizedException& authException) {
        callback(ResponseBuilder::error(authException.what(), drogon::k401Unauthorized));
    } catch (const nlohmann::json::exception& jsonException) {
        callback(ResponseBuilder::error(
            "Request body is not valid JSON or is missing required fields.",
            drogon::k400BadRequest
        ));
    } catch (const AppException& appException) {
        callback(ResponseBuilder::error(appException.what(), drogon::k500InternalServerError));
    }
}

void AuthController::changePassword(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback
) {
    try {
        const TokenClaims claims = RoleGuard::extractClaims(request);

        const auto jsonBody = nlohmann::json::parse(request->getBody());
        const auto changeRequest = ChangePasswordRequest::fromJson(jsonBody);

        authService->changePassword(claims.userId, changeRequest);

        callback(ResponseBuilder::success({{"success", true}, {"message", "Password changed successfully."}}));

    } catch (const UnauthorizedException& authException) {
        callback(ResponseBuilder::error(authException.what(), drogon::k401Unauthorized));
    } catch (const ValidationException& validationException) {
        callback(ResponseBuilder::error(validationException.what(), drogon::k400BadRequest));
    } catch (const NotFoundException& notFoundException) {
        callback(ResponseBuilder::error(notFoundException.what(), drogon::k404NotFound));
    } catch (const nlohmann::json::exception& jsonException) {
        callback(ResponseBuilder::error(
            "Request body is not valid JSON or is missing required fields.",
            drogon::k400BadRequest
        ));
    } catch (const AppException& appException) {
        callback(ResponseBuilder::error(appException.what(), drogon::k500InternalServerError));
    }
}

void AuthController::logout(
    const drogon::HttpRequestPtr& request,
    std::function<void(const drogon::HttpResponsePtr&)>&& callback
) {
    const TokenClaims claims   = RoleGuard::extractClaims(request);
    const std::string rawToken = request->getHeader("Authorization").substr(7);
    TokenBlacklist::getInstance().add(rawToken, claims.exp);

    callback(ResponseBuilder::success(
        {{"success", true}, {"message", "Logged out successfully."}}
    ));
}
