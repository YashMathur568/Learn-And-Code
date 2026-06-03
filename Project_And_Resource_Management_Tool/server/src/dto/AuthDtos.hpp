#pragma once

#include <nlohmann/json.hpp>
#include <string>

struct LoginRequest {
    std::string username;
    std::string password;

    static LoginRequest fromJson(const nlohmann::json& jsonBody) {
        LoginRequest request;
        request.username = jsonBody.at("username").get<std::string>();
        request.password = jsonBody.at("password").get<std::string>();
        return request;
    }
};

struct LoginResponse {
    std::string token;
    std::string role;
    bool        forcePasswordChange{false};

    nlohmann::json toJson() const {
        return {
            {"success",             true},
            {"token",               token},
            {"role",                role},
            {"forcePasswordChange", forcePasswordChange}
        };
    }
};

struct ChangePasswordRequest {
    std::string currentPassword;
    std::string newPassword;
    std::string confirmPassword;

    static ChangePasswordRequest fromJson(const nlohmann::json& jsonBody) {
        ChangePasswordRequest request;
        request.currentPassword = jsonBody.at("currentPassword").get<std::string>();
        request.newPassword     = jsonBody.at("newPassword").get<std::string>();
        request.confirmPassword = jsonBody.at("confirmPassword").get<std::string>();
        return request;
    }
};
