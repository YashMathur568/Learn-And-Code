#pragma once

#include "../services/UserService.hpp"
#include <drogon/HttpController.h>
#include <memory>

class UserController : public drogon::HttpController<UserController> {
public:
    UserController();

    METHOD_LIST_BEGIN
        ADD_METHOD_TO(UserController::getAllUsers,    "/api/admin/users",           drogon::Get,  "JwtMiddleware");
        ADD_METHOD_TO(UserController::createUser,    "/api/admin/users",           drogon::Post, "JwtMiddleware");
        ADD_METHOD_TO(UserController::deactivateUser,"/api/admin/users/{id}/deactivate", drogon::Put,  "JwtMiddleware");
        ADD_METHOD_TO(UserController::reactivateUser,"/api/admin/users/{id}/reactivate", drogon::Put,  "JwtMiddleware");
        ADD_METHOD_TO(UserController::resetPassword, "/api/admin/users/{id}/reset-password", drogon::Put, "JwtMiddleware");
    METHOD_LIST_END

    void getAllUsers(
        const drogon::HttpRequestPtr& request,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback
    );

    void createUser(
        const drogon::HttpRequestPtr& request,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback
    );

    void deactivateUser(
        const drogon::HttpRequestPtr& request,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback,
        int id
    );

    void reactivateUser(
        const drogon::HttpRequestPtr& request,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback,
        int id
    );

    void resetPassword(
        const drogon::HttpRequestPtr& request,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback,
        int id
    );

private:
    std::shared_ptr<UserService> userService;
};
