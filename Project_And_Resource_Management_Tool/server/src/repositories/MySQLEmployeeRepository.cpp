#include "MySQLEmployeeRepository.hpp"
#include "../utils/AppException.hpp"

#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/exception.h>

#include <memory>

// Joins users + roles + resource_profile + resource_status to produce Employee rows.
// resource_profile only exists for MANAGER/RESOURCE; LEFT JOIN includes ADMIN users.
// resource_status only exists for RESOURCE; LEFT JOIN tolerated as before.
static const std::string EMPLOYEE_SELECT =
    "SELECT u.user_id, rp.manager_id, ro.role_name AS role, u.full_name, u.email, "
    "COALESCE(rp.department, '') AS department, "
    "COALESCE(rp.designation, '') AS designation, "
    "COALESCE(rs.status, '') AS status, u.is_active "
    "FROM users u "
    "JOIN roles ro ON ro.role_id = u.role_id "
    "LEFT JOIN resource_profile rp ON rp.user_id = u.user_id "
    "LEFT JOIN resource_status rs ON rs.user_id = u.user_id ";

Employee MySQLEmployeeRepository::mapRowToEmployee(sql::ResultSet* resultSet) {
    Employee employee;
    employee.userId      = resultSet->getInt("user_id");
    employee.managerId   = resultSet->isNull("manager_id") ? 0 : resultSet->getInt("manager_id");
    employee.role        = resultSet->getString("role");
    employee.fullName    = resultSet->getString("full_name");
    employee.email       = resultSet->getString("email");
    employee.department  = resultSet->getString("department");
    employee.designation = resultSet->getString("designation");
    employee.status      = resultSet->getString("status");
    employee.isActive    = resultSet->getBoolean("is_active");
    return employee;
}

std::optional<Employee> MySQLEmployeeRepository::findById(int userId) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(EMPLOYEE_SELECT + "WHERE u.user_id = ?")
        );
        statement->setInt(1, userId);
        std::unique_ptr<sql::ResultSet> resultSet(statement->executeQuery());
        if (resultSet->next()) {
            return mapRowToEmployee(resultSet.get());
        }
        return std::nullopt;
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in employee findById: ") + sqlException.what());
    }
}

std::vector<Employee> MySQLEmployeeRepository::findAll() {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(EMPLOYEE_SELECT + "ORDER BY u.user_id")
        );
        std::unique_ptr<sql::ResultSet> resultSet(statement->executeQuery());
        std::vector<Employee> employees;
        while (resultSet->next()) {
            employees.push_back(mapRowToEmployee(resultSet.get()));
        }
        return employees;
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in employee findAll: ") + sqlException.what());
    }
}

std::vector<Employee> MySQLEmployeeRepository::findAllActive() {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(EMPLOYEE_SELECT + "WHERE u.is_active = 1 ORDER BY u.user_id")
        );
        std::unique_ptr<sql::ResultSet> resultSet(statement->executeQuery());
        std::vector<Employee> employees;
        while (resultSet->next()) {
            employees.push_back(mapRowToEmployee(resultSet.get()));
        }
        return employees;
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in findAllActive: ") + sqlException.what());
    }
}

std::vector<Employee> MySQLEmployeeRepository::findByManagerId(int managerUserId) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(EMPLOYEE_SELECT + "WHERE rp.manager_id = ? ORDER BY u.user_id")
        );
        statement->setInt(1, managerUserId);
        std::unique_ptr<sql::ResultSet> resultSet(statement->executeQuery());
        std::vector<Employee> employees;
        while (resultSet->next()) {
            employees.push_back(mapRowToEmployee(resultSet.get()));
        }
        return employees;
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in findByManagerId: ") + sqlException.what());
    }
}

int MySQLEmployeeRepository::create(const Employee& employee) {
    try {
        auto connection = DatabasePool::getInstance().acquire();

        // Insert resource_profile (covers both RESOURCE and MANAGER)
        std::unique_ptr<sql::PreparedStatement> profileStmt(
            connection->prepareStatement(
                "INSERT INTO resource_profile (user_id, manager_id, department, designation) "
                "VALUES (?, ?, ?, ?)"
            )
        );
        profileStmt->setInt(1, employee.userId);
        if (employee.managerId > 0) {
            profileStmt->setInt(2, employee.managerId);
        } else {
            profileStmt->setNull(2, 0);
        }
        profileStmt->setString(3, employee.department);
        profileStmt->setString(4, employee.designation);
        profileStmt->executeUpdate();

        // Insert resource_status only for RESOURCE (status field non-empty means RESOURCE)
        if (!employee.status.empty()) {
            std::unique_ptr<sql::PreparedStatement> statusStmt(
                connection->prepareStatement(
                    "INSERT INTO resource_status (user_id, status) VALUES (?, 'BENCH')"
                )
            );
            statusStmt->setInt(1, employee.userId);
            statusStmt->executeUpdate();
        }

        return employee.userId;
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in employee create: ") + sqlException.what());
    }
}

void MySQLEmployeeRepository::update(const Employee& employee) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                "UPDATE resource_profile SET department = ?, designation = ? "
                "WHERE user_id = ?"
            )
        );
        statement->setString(1, employee.department);
        statement->setString(2, employee.designation);
        statement->setInt(3, employee.userId);
        statement->executeUpdate();
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in employee update: ") + sqlException.what());
    }
}

void MySQLEmployeeRepository::setActiveStatus(int userId, bool active) {
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
        throw AppException(std::string("DB error in employee setActiveStatus: ") + sqlException.what());
    }
}

void MySQLEmployeeRepository::setStatus(int userId, const std::string& status) {
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
        throw AppException(std::string("DB error in employee setStatus: ") + sqlException.what());
    }
}

bool MySQLEmployeeRepository::hasActiveAllocations(int userId) {
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

bool MySQLEmployeeRepository::existsByUserId(int userId) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                "SELECT COUNT(*) FROM resource_profile WHERE user_id = ?"
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

void MySQLEmployeeRepository::assignManager(int userId, int managerUserId) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                "UPDATE resource_profile SET manager_id = ? WHERE user_id = ?"
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
