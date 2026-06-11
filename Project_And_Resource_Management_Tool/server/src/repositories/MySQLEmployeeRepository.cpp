#include "MySQLEmployeeRepository.hpp"
#include "../utils/AppException.hpp"

#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/exception.h>

#include <memory>

Employee MySQLEmployeeRepository::mapRowToEmployee(sql::ResultSet* resultSet) {
    Employee employee;
    employee.employeeId  = resultSet->getInt("employee_id");
    employee.userId      = resultSet->getInt("user_id");
    employee.managerId   = resultSet->isNull("manager_id") ? 0 : resultSet->getInt("manager_id");
    employee.fullName    = resultSet->getString("full_name");
    employee.email       = resultSet->getString("email");
    employee.department  = resultSet->getString("department");
    employee.designation = resultSet->getString("designation");
    employee.status      = resultSet->getString("status");
    employee.isActive    = resultSet->getBoolean("is_active");
    return employee;
}

static const std::string SELECT_COLUMNS =
    "SELECT employee_id, user_id, manager_id, full_name, email, department, designation, status, is_active "
    "FROM employees ";

std::optional<Employee> MySQLEmployeeRepository::findById(int employeeId) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(SELECT_COLUMNS + "WHERE employee_id = ?")
        );
        statement->setInt(1, employeeId);
        std::unique_ptr<sql::ResultSet> resultSet(statement->executeQuery());
        if (resultSet->next()) {
            return mapRowToEmployee(resultSet.get());
        }
        return std::nullopt;
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in employee findById: ") + sqlException.what());
    }
}

std::optional<Employee> MySQLEmployeeRepository::findByUserId(int userId) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(SELECT_COLUMNS + "WHERE user_id = ?")
        );
        statement->setInt(1, userId);
        std::unique_ptr<sql::ResultSet> resultSet(statement->executeQuery());
        if (resultSet->next()) {
            return mapRowToEmployee(resultSet.get());
        }
        return std::nullopt;
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in findByUserId: ") + sqlException.what());
    }
}

std::vector<Employee> MySQLEmployeeRepository::findAll() {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(SELECT_COLUMNS + "ORDER BY employee_id")
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
            connection->prepareStatement(SELECT_COLUMNS + "WHERE is_active = 1 ORDER BY employee_id")
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

int MySQLEmployeeRepository::create(const Employee& employee) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                "INSERT INTO employees (user_id, manager_id, full_name, email, department, designation, status, is_active) "
                "VALUES (?, ?, ?, ?, ?, ?, 'BENCH', 1)"
            )
        );
        statement->setInt(1, employee.userId);
        if (employee.managerId > 0) {
            statement->setInt(2, employee.managerId);
        } else {
            statement->setNull(2, 0);
        }
        statement->setString(3, employee.fullName);
        statement->setString(4, employee.email);
        statement->setString(5, employee.department);
        statement->setString(6, employee.designation);
        statement->executeUpdate();

        std::unique_ptr<sql::PreparedStatement> idStatement(
            connection->prepareStatement("SELECT LAST_INSERT_ID()")
        );
        std::unique_ptr<sql::ResultSet> idResult(idStatement->executeQuery());
        idResult->next();
        return idResult->getInt(1);
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in employee create: ") + sqlException.what());
    }
}

void MySQLEmployeeRepository::update(const Employee& employee) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                "UPDATE employees SET full_name = ?, email = ?, department = ?, designation = ? "
                "WHERE employee_id = ?"
            )
        );
        statement->setString(1, employee.fullName);
        statement->setString(2, employee.email);
        statement->setString(3, employee.department);
        statement->setString(4, employee.designation);
        statement->setInt(5, employee.employeeId);
        statement->executeUpdate();
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in employee update: ") + sqlException.what());
    }
}

void MySQLEmployeeRepository::setActiveStatus(int employeeId, bool active) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                "UPDATE employees SET is_active = ? WHERE employee_id = ?"
            )
        );
        statement->setBoolean(1, active);
        statement->setInt(2, employeeId);
        statement->executeUpdate();
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in employee setActiveStatus: ") + sqlException.what());
    }
}

void MySQLEmployeeRepository::setStatus(int employeeId, const std::string& status) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                "UPDATE employees SET status = ? WHERE employee_id = ?"
            )
        );
        statement->setString(1, status);
        statement->setInt(2, employeeId);
        statement->executeUpdate();
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in employee setStatus: ") + sqlException.what());
    }
}

bool MySQLEmployeeRepository::hasActiveAllocations(int employeeId) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                "SELECT COUNT(*) FROM allocations "
                "WHERE employee_id = ? AND is_active = 1 AND to_date >= CURDATE()"
            )
        );
        statement->setInt(1, employeeId);
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
                "SELECT COUNT(*) FROM employees WHERE user_id = ?"
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

std::vector<Employee> MySQLEmployeeRepository::findByManagerId(int managerEmployeeId) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                SELECT_COLUMNS + "WHERE manager_id = ? ORDER BY employee_id"
            )
        );
        statement->setInt(1, managerEmployeeId);
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

void MySQLEmployeeRepository::assignManager(int employeeId, int managerEmployeeId) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                "UPDATE employees SET manager_id = ? WHERE employee_id = ?"
            )
        );
        if (managerEmployeeId > 0) {
            statement->setInt(1, managerEmployeeId);
        } else {
            statement->setNull(1, 0);
        }
        statement->setInt(2, employeeId);
        statement->executeUpdate();
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in assignManager: ") + sqlException.what());
    }
}
