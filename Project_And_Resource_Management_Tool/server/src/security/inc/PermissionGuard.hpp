#pragma once

#include <drogon/HttpRequest.h>
#include <initializer_list>
#include <string>



class PermissionGuard {
public:

    static void require(const drogon::HttpRequestPtr& request,
                        const std::string& permissionName);


    static void requireAny(const drogon::HttpRequestPtr& request,
                           std::initializer_list<std::string> permissionNames);

private:
    PermissionGuard() = delete;
};
