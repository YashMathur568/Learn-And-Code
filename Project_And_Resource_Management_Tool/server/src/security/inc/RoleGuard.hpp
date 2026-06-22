#pragma once

#include "TokenClaims.hpp"
#include <drogon/HttpRequest.h>
#include <string>

class RoleGuard {
public:
    static TokenClaims extractClaims(const drogon::HttpRequestPtr& request);
    static void        requireRole(const drogon::HttpRequestPtr& request,
                                   const std::string& requiredRole);
    static void        requireAnyRole(const drogon::HttpRequestPtr& request,
                                      std::initializer_list<std::string> allowedRoles);

private:
    RoleGuard() = delete;

    static constexpr auto CLAIMS_ATTRIBUTE_KEY = "tokenClaims";
};
