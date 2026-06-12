#include "MySQLPermissionRepository.hpp"
#include "../utils/AppException.hpp"

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
                "SELECT COUNT(*) FROM role_permissions rp "
                "JOIN roles r       ON r.role_id       = rp.role_id "
                "JOIN permissions p ON p.permission_id = rp.permission_id "
                "WHERE r.role_name       = ? "
                "  AND p.permission_name = ? "
                "  AND (rp.expires_at IS NULL OR rp.expires_at > NOW())"
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
