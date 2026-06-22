#include "RoleGuard.hpp"
#include "AppException.hpp"

#include <algorithm>
#include <initializer_list>
#include <memory>
#include <string>

TokenClaims RoleGuard::extractClaims(const drogon::HttpRequestPtr& request) {
    auto claimsPtr = request->getAttributes()->get<std::shared_ptr<TokenClaims>>(CLAIMS_ATTRIBUTE_KEY);
    if (!claimsPtr) {
        throw UnauthorizedException("Request has no token claims. JwtMiddleware must run first.");
    }
    return *claimsPtr;
}

void RoleGuard::requireRole(const drogon::HttpRequestPtr& request,
                             const std::string& requiredRole) {
    TokenClaims claims = extractClaims(request);
    if (claims.role != requiredRole) {
        throw UnauthorizedException(
            "Access denied. Required role: " + requiredRole +
            ". Your role: " + claims.role
        );
    }
}

void RoleGuard::requireAnyRole(const drogon::HttpRequestPtr& request,
                                std::initializer_list<std::string> allowedRoles) {
    TokenClaims claims = extractClaims(request);

    bool roleMatches = std::any_of(
        allowedRoles.begin(),
        allowedRoles.end(),
        [&claims](const std::string& allowedRole) {
            return claims.role == allowedRole;
        }
    );

    if (!roleMatches) {
        throw UnauthorizedException("Access denied. Insufficient role: " + claims.role);
    }
}
