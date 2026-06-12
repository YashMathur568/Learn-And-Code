#include "PermissionGuard.hpp"
#include "RoleGuard.hpp"
#include "../repositories/MySQLPermissionRepository.hpp"
#include "../utils/AppException.hpp"

void PermissionGuard::require(const drogon::HttpRequestPtr& request,
                               const std::string& permissionName) {
    TokenClaims claims = RoleGuard::extractClaims(request);

    MySQLPermissionRepository permRepo;
    if (!permRepo.roleHasPermission(claims.role, permissionName)) {
        throw ForbiddenException(
            "Permission denied. Role '" + claims.role +
            "' does not have permission: " + permissionName
        );
    }
}
