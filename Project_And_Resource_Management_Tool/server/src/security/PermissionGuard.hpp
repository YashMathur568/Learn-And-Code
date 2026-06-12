#pragma once

#include <drogon/HttpRequest.h>
#include <string>

// Checks that the caller's role has the given permission in the DB.
// Throws ForbiddenException if not.
class PermissionGuard {
public:
    static void require(const drogon::HttpRequestPtr& request,
                        const std::string& permissionName);

private:
    PermissionGuard() = delete;
};
