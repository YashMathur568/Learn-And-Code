#include "PermissionGuard.hpp"
#include "RoleGuard.hpp"
#include "MySQLPermissionRepository.hpp"
#include "AppException.hpp"

void PermissionGuard::require(const drogon::HttpRequestPtr& request,
                               const std::string& permissionName) {
    const TokenClaims claims = RoleGuard::extractClaims(request);
    MySQLPermissionRepository permRepo;
    if (!permRepo.roleHasPermission(claims.role, permissionName)) {
        throw ForbiddenException(
            "Permission denied. Role '" + claims.role +
            "' does not have permission: " + permissionName
        );
    }
}

void PermissionGuard::requireAny(const drogon::HttpRequestPtr& request,
                                  std::initializer_list<std::string> permissionNames) {
    const TokenClaims claims = RoleGuard::extractClaims(request);
    MySQLPermissionRepository permRepo;
    for (const auto& perm : permissionNames) {
        if (permRepo.roleHasPermission(claims.role, perm)) return;
    }
    throw ForbiddenException(
        "Permission denied. Role '" + claims.role +
        "' does not have any of the required permissions."
    );
}
