#pragma once

#include "../services/IAuthService.hpp"
#include "../security/JwtMiddleware.hpp"

#include <drogon/HttpController.h>
#include <memory>

class AuthController : public drogon::HttpController<AuthController> {
public:
    explicit AuthController();

    METHOD_LIST_BEGIN
        ADD_METHOD_TO(AuthController::login,
                      "/api/auth/login",
                      drogon::Post);

        ADD_METHOD_TO(AuthController::changePassword,
                      "/api/auth/change-password",
                      drogon::Post,
                      "JwtMiddleware");

        ADD_METHOD_TO(AuthController::logout,
                      "/api/auth/logout",
                      drogon::Post,
                      "JwtMiddleware");
    METHOD_LIST_END

    void login(const drogon::HttpRequestPtr& request,
               std::function<void(const drogon::HttpResponsePtr&)>&& callback);

    void changePassword(const drogon::HttpRequestPtr& request,
                        std::function<void(const drogon::HttpResponsePtr&)>&& callback);

    void logout(const drogon::HttpRequestPtr& request,
                std::function<void(const drogon::HttpResponsePtr&)>&& callback);

private:
    std::shared_ptr<IAuthService> authService;
};
