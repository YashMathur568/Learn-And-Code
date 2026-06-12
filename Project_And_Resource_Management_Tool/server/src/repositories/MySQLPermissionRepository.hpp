#pragma once

#include "IPermissionRepository.hpp"
#include "../utils/DatabasePool.hpp"

class MySQLPermissionRepository : public IPermissionRepository {
public:
    MySQLPermissionRepository() = default;

    bool roleHasPermission(const std::string& role,
                           const std::string& permissionName) override;
};
