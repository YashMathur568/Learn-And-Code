#include "MySQLResourceRepository.hpp"
#include "AppException.hpp"

#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/exception.h>

#include <memory>




static const std::string RESOURCE_SELECT =
    "SELECT u_users.user_id, rp_user_profile.manager_id, ro_roles.role_name AS role, u_users.full_name, u_users.email, "
    "COALESCE(rp_user_profile.department, '') AS department, "
    "COALESCE(rp_user_profile.designation, '') AS designation, "
    "COALESCE(rs_resource_status.status, '') AS status, u_users.is_active, u_users.is_frozen "
    "FROM users u_users "
    "JOIN roles ro_roles ON ro_roles.role_id = u_users.role_id "
    "LEFT JOIN user_profile rp_user_profile ON rp_user_profile.user_id = u_users.user_id "
    "LEFT JOIN resource_status rs_resource_status ON rs_resource_status.user_id = u_users.user_id ";

Resource MySQLResourceRepository::mapRowToResource(sql::ResultSet* resultSet) {
    Resource resource;
    resource.userId      = resultSet->getInt("user_id");
    resource.managerId   = resultSet->isNull("manager_id") ? 0 : resultSet->getInt("manager_id");
    resource.role        = resultSet->getString("role");
    resource.fullName    = resultSet->getString("full_name");
    resource.email       = resultSet->getString("email");
    resource.department  = resultSet->getString("department");
    resource.designation = resultSet->getString("designation");
    resource.status      = resultSet->getString("status");
    resource.isActive    = resultSet->getBoolean("is_active");
    resource.isFrozen    = resultSet->getBoolean("is_frozen");
    return resource;
}

std::optional<Resource> MySQLResourceRepository::findById(int userId) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(RESOURCE_SELECT + "WHERE u_users.user_id = ?")
        );
        statement->setInt(1, userId);
        std::unique_ptr<sql::ResultSet> resultSet(statement->executeQuery());
        if (resultSet->next()) {
            return mapRowToResource(resultSet.get());
        }
        return std::nullopt;
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in resource findById: ") + sqlException.what());
    }
}

std::vector<Resource> MySQLResourceRepository::findAll() {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(RESOURCE_SELECT + "ORDER BY u_users.user_id")
        );
        std::unique_ptr<sql::ResultSet> resultSet(statement->executeQuery());
        std::vector<Resource> resources;
        while (resultSet->next()) {
            resources.push_back(mapRowToResource(resultSet.get()));
        }
        return resources;
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in resource findAll: ") + sqlException.what());
    }
}

std::vector<Resource> MySQLResourceRepository::findAllActive() {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(RESOURCE_SELECT + "WHERE u_users.is_active = 1 ORDER BY u_users.user_id")
        );
        std::unique_ptr<sql::ResultSet> resultSet(statement->executeQuery());
        std::vector<Resource> resources;
        while (resultSet->next()) {
            resources.push_back(mapRowToResource(resultSet.get()));
        }
        return resources;
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in findAllActive: ") + sqlException.what());
    }
}

std::vector<Resource> MySQLResourceRepository::findByManagerId(int managerUserId) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(RESOURCE_SELECT + "WHERE rp_user_profile.manager_id = ? ORDER BY u_users.user_id")
        );
        statement->setInt(1, managerUserId);
        std::unique_ptr<sql::ResultSet> resultSet(statement->executeQuery());
        std::vector<Resource> resources;
        while (resultSet->next()) {
            resources.push_back(mapRowToResource(resultSet.get()));
        }
        return resources;
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in findByManagerId: ") + sqlException.what());
    }
}

int MySQLResourceRepository::create(const Resource& resource) {
    try {
        auto connection = DatabasePool::getInstance().acquire();


        std::unique_ptr<sql::PreparedStatement> profileStmt(
            connection->prepareStatement(
                "INSERT INTO user_profile (user_id, manager_id, department, designation) "
                "VALUES (?, ?, ?, ?)"
            )
        );
        profileStmt->setInt(1, resource.userId);
        if (resource.managerId > 0) {
            profileStmt->setInt(2, resource.managerId);
        } else {
            profileStmt->setNull(2, 0);
        }
        profileStmt->setString(3, resource.department);
        profileStmt->setString(4, resource.designation);
        profileStmt->executeUpdate();


        if (!resource.status.empty()) {
            std::unique_ptr<sql::PreparedStatement> statusStmt(
                connection->prepareStatement(
                    "INSERT INTO resource_status (user_id, status) VALUES (?, 'BENCH')"
                )
            );
            statusStmt->setInt(1, resource.userId);
            statusStmt->executeUpdate();
        }

        return resource.userId;
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in resource create: ") + sqlException.what());
    }
}

void MySQLResourceRepository::update(const Resource& resource) {
    try {
        auto connection = DatabasePool::getInstance().acquire();

        std::unique_ptr<sql::PreparedStatement> userStatement(
            connection->prepareStatement(
                "UPDATE users SET full_name = ?, email = ? WHERE user_id = ?"
            )
        );
        userStatement->setString(1, resource.fullName);
        userStatement->setString(2, resource.email);
        userStatement->setInt(3, resource.userId);
        userStatement->executeUpdate();

        std::unique_ptr<sql::PreparedStatement> profileStatement(
            connection->prepareStatement(
                "UPDATE user_profile SET department = ?, designation = ? "
                "WHERE user_id = ?"
            )
        );
        profileStatement->setString(1, resource.department);
        profileStatement->setString(2, resource.designation);
        profileStatement->setInt(3, resource.userId);
        profileStatement->executeUpdate();
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in resource update: ") + sqlException.what());
    }
}

void MySQLResourceRepository::setActiveStatus(int userId, bool active) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                "UPDATE users SET is_active = ? WHERE user_id = ?"
            )
        );
        statement->setBoolean(1, active);
        statement->setInt(2, userId);
        statement->executeUpdate();
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in resource setActiveStatus: ") + sqlException.what());
    }
}

void MySQLResourceRepository::setStatus(int userId, const std::string& status) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                "UPDATE resource_status SET status = ? WHERE user_id = ?"
            )
        );
        statement->setString(1, status);
        statement->setInt(2, userId);
        statement->executeUpdate();
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in resource setStatus: ") + sqlException.what());
    }
}

bool MySQLResourceRepository::hasActiveAllocations(int userId) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                "SELECT COUNT(*) FROM allocations "
                "WHERE user_id = ? AND is_active = 1 AND `to_date` >= CURDATE()"
            )
        );
        statement->setInt(1, userId);
        std::unique_ptr<sql::ResultSet> resultSet(statement->executeQuery());
        resultSet->next();
        return resultSet->getInt(1) > 0;
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in hasActiveAllocations: ") + sqlException.what());
    }
}

bool MySQLResourceRepository::existsByUserId(int userId) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                "SELECT COUNT(*) FROM user_profile WHERE user_id = ?"
            )
        );
        statement->setInt(1, userId);
        std::unique_ptr<sql::ResultSet> resultSet(statement->executeQuery());
        resultSet->next();
        return resultSet->getInt(1) > 0;
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in existsByUserId: ") + sqlException.what());
    }
}

void MySQLResourceRepository::setFrozen(int userId, bool frozen) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement("UPDATE users SET is_frozen = ? WHERE user_id = ?")
        );
        statement->setInt(1, frozen ? 1 : 0);
        statement->setInt(2, userId);
        statement->executeUpdate();
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in setFrozen: ") + sqlException.what());
    }
}

bool MySQLResourceRepository::isFrozen(int userId) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement("SELECT is_frozen FROM users WHERE user_id = ?")
        );
        statement->setInt(1, userId);
        std::unique_ptr<sql::ResultSet> resultSet(statement->executeQuery());
        if (resultSet->next()) return resultSet->getBoolean("is_frozen");
        return false;
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in isFrozen: ") + sqlException.what());
    }
}

void MySQLResourceRepository::assignManager(int userId, int managerUserId) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                "UPDATE user_profile SET manager_id = ? WHERE user_id = ?"
            )
        );
        if (managerUserId > 0) {
            statement->setInt(1, managerUserId);
        } else {
            statement->setNull(1, 0);
        }
        statement->setInt(2, userId);
        statement->executeUpdate();
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in assignManager: ") + sqlException.what());
    }
}

void MySQLResourceRepository::deleteResourceStatus(int userId) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement("DELETE FROM resource_status WHERE user_id = ?")
        );
        statement->setInt(1, userId);
        statement->executeUpdate();
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in deleteResourceStatus: ") + sqlException.what());
    }
}

void MySQLResourceRepository::createResourceStatus(int userId) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                "INSERT INTO resource_status (user_id, status) VALUES (?, 'BENCH')"
            )
        );
        statement->setInt(1, userId);
        statement->executeUpdate();
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in createResourceStatus: ") + sqlException.what());
    }
}

int MySQLResourceRepository::countManagedProjects(int userId) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                "SELECT COUNT(*) FROM projects WHERE manager_id = ?"
            )
        );
        statement->setInt(1, userId);
        std::unique_ptr<sql::ResultSet> resultSet(statement->executeQuery());
        resultSet->next();
        return resultSet->getInt(1);
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in countManagedProjects: ") + sqlException.what());
    }
}
