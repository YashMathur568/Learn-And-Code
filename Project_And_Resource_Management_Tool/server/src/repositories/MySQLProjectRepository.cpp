#include "MySQLProjectRepository.hpp"
#include "../utils/AppException.hpp"

#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/exception.h>

#include <memory>

Project MySQLProjectRepository::mapRowToProject(sql::ResultSet* resultSet) {
    Project project;
    project.projectId   = resultSet->getInt("project_id");
    project.name        = resultSet->getString("name").asStdString();
    project.description = resultSet->getString("description").asStdString();
    project.startDate   = resultSet->getString("start_date").asStdString();
    project.endDate     = resultSet->getString("end_date").asStdString();
    project.status      = resultSet->getString("status").asStdString();
    project.managerId   = resultSet->getInt("manager_id");
    project.health      = resultSet->getString("health").asStdString();
    return project;
}

static const std::string PROJECT_SELECT =
    "SELECT project_id, name, description, start_date, end_date, status, manager_id, health "
    "FROM projects ";

std::optional<Project> MySQLProjectRepository::findById(int projectId) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(PROJECT_SELECT + "WHERE project_id = ?")
        );
        statement->setInt(1, projectId);
        std::unique_ptr<sql::ResultSet> resultSet(statement->executeQuery());
        if (resultSet->next()) {
            return mapRowToProject(resultSet.get());
        }
        return std::nullopt;
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in project findById: ") + sqlException.what());
    }
}

std::vector<Project> MySQLProjectRepository::findAll() {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(PROJECT_SELECT + "ORDER BY project_id")
        );
        std::unique_ptr<sql::ResultSet> resultSet(statement->executeQuery());
        std::vector<Project> projects;
        while (resultSet->next()) {
            projects.push_back(mapRowToProject(resultSet.get()));
        }
        return projects;
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in project findAll: ") + sqlException.what());
    }
}

std::vector<Project> MySQLProjectRepository::findByManagerId(int managerId) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(PROJECT_SELECT + "WHERE manager_id = ? ORDER BY project_id")
        );
        statement->setInt(1, managerId);
        std::unique_ptr<sql::ResultSet> resultSet(statement->executeQuery());
        std::vector<Project> projects;
        while (resultSet->next()) {
            projects.push_back(mapRowToProject(resultSet.get()));
        }
        return projects;
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in findByManagerId: ") + sqlException.what());
    }
}

int MySQLProjectRepository::create(const Project& project) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                "INSERT INTO projects (name, description, start_date, end_date, status, manager_id, health) "
                "VALUES (?, ?, ?, ?, ?, ?, 'ON_TRACK')"
            )
        );
        statement->setString(1, project.name);
        statement->setString(2, project.description);
        statement->setString(3, project.startDate);
        statement->setString(4, project.endDate);
        statement->setString(5, project.status);
        statement->setInt(6, project.managerId);
        statement->executeUpdate();

        std::unique_ptr<sql::PreparedStatement> idStatement(
            connection->prepareStatement("SELECT LAST_INSERT_ID()")
        );
        std::unique_ptr<sql::ResultSet> idResult(idStatement->executeQuery());
        idResult->next();
        return idResult->getInt(1);
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in project create: ") + sqlException.what());
    }
}

void MySQLProjectRepository::update(const Project& project) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                "UPDATE projects SET name = ?, description = ?, start_date = ?, "
                "end_date = ?, status = ?, manager_id = ? WHERE project_id = ?"
            )
        );
        statement->setString(1, project.name);
        statement->setString(2, project.description);
        statement->setString(3, project.startDate);
        statement->setString(4, project.endDate);
        statement->setString(5, project.status);
        statement->setInt(6, project.managerId);
        statement->setInt(7, project.projectId);
        statement->executeUpdate();
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in project update: ") + sqlException.what());
    }
}

void MySQLProjectRepository::updateHealth(int projectId, const std::string& health) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                "UPDATE projects SET health = ? WHERE project_id = ?"
            )
        );
        statement->setString(1, health);
        statement->setInt(2, projectId);
        statement->executeUpdate();
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in updateHealth: ") + sqlException.what());
    }
}

bool MySQLProjectRepository::managerExists(int employeeId) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                "SELECT COUNT(*) FROM employees WHERE employee_id = ? AND is_active = 1"
            )
        );
        statement->setInt(1, employeeId);
        std::unique_ptr<sql::ResultSet> resultSet(statement->executeQuery());
        resultSet->next();
        return resultSet->getInt(1) > 0;
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in managerExists: ") + sqlException.what());
    }
}
