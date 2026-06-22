#pragma once

#include <string>

class IPermissionRepository {
public:
    virtual ~IPermissionRepository() = default;

    virtual bool roleHasPermission(const std::string& role,
                                   const std::string& permissionName) = 0;
};
