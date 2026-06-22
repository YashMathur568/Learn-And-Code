#include "MySQLPermissionRepository.hpp"
#include "AppException.hpp"

#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/exception.h>

#include <memory>

bool MySQLPermissionRepository::roleHasPermission(const std::string& role,
                                                   const std::string& permissionName) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                "SELECT COUNT(*) FROM role_permissions rp_role_permissions "
                "JOIN roles r_roles             ON r_roles.role_id             = rp_role_permissions.role_id "
                "JOIN permissions p_permissions ON p_permissions.permission_id = rp_role_permissions.permission_id "
                "WHERE r_roles.role_name       = ? "
                "  AND p_permissions.permission_name = ? "
                "  AND (rp_role_permissions.expires_at IS NULL OR rp_role_permissions.expires_at > NOW())"
            )
        );
        statement->setString(1, role);
        statement->setString(2, permissionName);
        std::unique_ptr<sql::ResultSet> resultSet(statement->executeQuery());
        resultSet->next();
        return resultSet->getInt(1) > 0;
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in roleHasPermission: ") + sqlException.what());
    }
}
