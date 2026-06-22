#include "MySQLAllocationRepository.hpp"
#include "AppException.hpp"

#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/exception.h>

#include <memory>

Allocation MySQLAllocationRepository::mapRowToAllocation(sql::ResultSet* resultSet) {
    Allocation allocation;
    allocation.allocationId = resultSet->getInt("allocation_id");
    allocation.userId       = resultSet->getInt("user_id");
    allocation.projectId    = resultSet->getInt("project_id");
    allocation.projectName  = resultSet->getString("project_name");
    allocation.utilisation  = resultSet->getInt("utilisation");
    allocation.fromDate     = resultSet->getString("from_date");
    allocation.toDate       = resultSet->getString("to_date");
    allocation.isActive     = resultSet->getBoolean("is_active");
    return allocation;
}

static const std::string ALLOC_SELECT =
    "SELECT alloc_allocations.allocation_id, alloc_allocations.user_id, alloc_allocations.project_id, proj_projects.name AS project_name, "
    "alloc_allocations.utilisation, alloc_allocations.from_date, alloc_allocations.`to_date`, alloc_allocations.is_active "
    "FROM allocations alloc_allocations JOIN projects proj_projects ON proj_projects.project_id = alloc_allocations.project_id ";

std::optional<Allocation> MySQLAllocationRepository::findById(int allocationId) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(ALLOC_SELECT + "WHERE alloc_allocations.allocation_id = ?")
        );
        statement->setInt(1, allocationId);
        std::unique_ptr<sql::ResultSet> resultSet(statement->executeQuery());
        if (resultSet->next()) {
            return mapRowToAllocation(resultSet.get());
        }
        return std::nullopt;
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in allocation findById: ") + sqlException.what());
    }
}

std::vector<Allocation> MySQLAllocationRepository::findByUserId(int userId) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(ALLOC_SELECT + "WHERE alloc_allocations.user_id = ? ORDER BY alloc_allocations.from_date DESC")
        );
        statement->setInt(1, userId);
        std::unique_ptr<sql::ResultSet> resultSet(statement->executeQuery());
        std::vector<Allocation> allocations;
        while (resultSet->next()) {
            allocations.push_back(mapRowToAllocation(resultSet.get()));
        }
        return allocations;
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in allocation findByUserId: ") + sqlException.what());
    }
}

std::vector<Allocation> MySQLAllocationRepository::findActiveByUserId(int userId) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                ALLOC_SELECT + "WHERE alloc_allocations.user_id = ? AND alloc_allocations.is_active = 1 AND alloc_allocations.`to_date` >= CURDATE() ORDER BY alloc_allocations.from_date"
            )
        );
        statement->setInt(1, userId);
        std::unique_ptr<sql::ResultSet> resultSet(statement->executeQuery());
        std::vector<Allocation> allocations;
        while (resultSet->next()) {
            allocations.push_back(mapRowToAllocation(resultSet.get()));
        }
        return allocations;
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in findActiveByUserId: ") + sqlException.what());
    }
}

std::vector<Allocation> MySQLAllocationRepository::findActiveByProjectId(int projectId) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                ALLOC_SELECT + "WHERE alloc_allocations.project_id = ? AND alloc_allocations.is_active = 1 AND alloc_allocations.`to_date` >= CURDATE() ORDER BY alloc_allocations.user_id"
            )
        );
        statement->setInt(1, projectId);
        std::unique_ptr<sql::ResultSet> resultSet(statement->executeQuery());
        std::vector<Allocation> allocations;
        while (resultSet->next()) {
            allocations.push_back(mapRowToAllocation(resultSet.get()));
        }
        return allocations;
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in findActiveByProjectId: ") + sqlException.what());
    }
}

int MySQLAllocationRepository::getTotalActiveUtilisation(int userId) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                "SELECT COALESCE(SUM(utilisation), 0) AS total "
                "FROM allocations "
                "WHERE user_id = ? AND is_active = 1 AND `to_date` >= CURDATE()"
            )
        );
        statement->setInt(1, userId);
        std::unique_ptr<sql::ResultSet> resultSet(statement->executeQuery());
        resultSet->next();
        return resultSet->getInt("total");
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in getTotalActiveUtilisation: ") + sqlException.what());
    }
}

bool MySQLAllocationRepository::wasAllocatedDuringWeek(int userId, int projectId, const std::string& weekStart) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                "SELECT COUNT(*) FROM allocations "
                "WHERE user_id = ? AND project_id = ? AND from_date <= ? AND `to_date` >= ?"
            )
        );
        statement->setInt(1, userId);
        statement->setInt(2, projectId);
        statement->setString(3, weekStart);
        statement->setString(4, weekStart);
        std::unique_ptr<sql::ResultSet> resultSet(statement->executeQuery());
        resultSet->next();
        return resultSet->getInt(1) > 0;
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in wasAllocatedDuringWeek: ") + sqlException.what());
    }
}

std::string MySQLAllocationRepository::getProjectStatus(int projectId) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement("SELECT status FROM projects WHERE project_id = ?")
        );
        statement->setInt(1, projectId);
        std::unique_ptr<sql::ResultSet> resultSet(statement->executeQuery());
        if (resultSet->next()) {
            return resultSet->getString("status").c_str();
        }
        throw AppException("Project not found: " + std::to_string(projectId));
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in getProjectStatus: ") + sqlException.what());
    }
}

int MySQLAllocationRepository::create(const Allocation& allocation) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                "INSERT INTO allocations (user_id, project_id, utilisation, from_date, `to_date`, is_active) "
                "VALUES (?, ?, ?, ?, ?, 1)"
            )
        );
        statement->setInt(1, allocation.userId);
        statement->setInt(2, allocation.projectId);
        statement->setInt(3, allocation.utilisation);
        statement->setString(4, allocation.fromDate);
        statement->setString(5, allocation.toDate);
        statement->executeUpdate();

        std::unique_ptr<sql::PreparedStatement> idStatement(
            connection->prepareStatement("SELECT LAST_INSERT_ID()")
        );
        std::unique_ptr<sql::ResultSet> idResult(idStatement->executeQuery());
        idResult->next();
        return idResult->getInt(1);
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in allocation create: ") + sqlException.what());
    }
}

void MySQLAllocationRepository::end(int allocationId) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                "UPDATE allocations SET is_active = 0 WHERE allocation_id = ?"
            )
        );
        statement->setInt(1, allocationId);
        statement->executeUpdate();
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in allocation end: ") + sqlException.what());
    }
}

void MySQLAllocationRepository::endAllByUser(int userId) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                "UPDATE allocations SET is_active = 0, `to_date` = CURDATE() "
                "WHERE user_id = ? AND is_active = 1"
            )
        );
        statement->setInt(1, userId);
        statement->executeUpdate();
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in endAllByUser: ") + sqlException.what());
    }
}
