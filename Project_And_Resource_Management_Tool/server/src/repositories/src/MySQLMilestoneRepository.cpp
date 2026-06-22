#include "MySQLMilestoneRepository.hpp"
#include "AppException.hpp"

#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/exception.h>

#include <memory>

Milestone MySQLMilestoneRepository::mapRowToMilestone(sql::ResultSet* resultSet) {
    Milestone milestone;
    milestone.milestoneId  = resultSet->getInt("milestone_id");
    milestone.projectId    = resultSet->getInt("project_id");
    milestone.title        = resultSet->getString("title");
    milestone.dueDate      = resultSet->getString("due_date");
    milestone.status       = resultSet->getString("status");
    milestone.storyPoints  = resultSet->getInt("story_points");
    return milestone;
}

std::optional<Milestone> MySQLMilestoneRepository::findById(int milestoneId) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                "SELECT milestone_id, project_id, title, due_date, status, story_points "
                "FROM milestones WHERE milestone_id = ?"
            )
        );
        statement->setInt(1, milestoneId);
        std::unique_ptr<sql::ResultSet> resultSet(statement->executeQuery());
        if (resultSet->next()) {
            return mapRowToMilestone(resultSet.get());
        }
        return std::nullopt;
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in milestone findById: ") + sqlException.what());
    }
}

std::vector<Milestone> MySQLMilestoneRepository::findByProjectId(int projectId) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                "SELECT milestone_id, project_id, title, due_date, status, story_points "
                "FROM milestones WHERE project_id = ? ORDER BY due_date"
            )
        );
        statement->setInt(1, projectId);
        std::unique_ptr<sql::ResultSet> resultSet(statement->executeQuery());
        std::vector<Milestone> milestones;
        while (resultSet->next()) {
            milestones.push_back(mapRowToMilestone(resultSet.get()));
        }
        return milestones;
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in findByProjectId: ") + sqlException.what());
    }
}

int MySQLMilestoneRepository::create(const Milestone& milestone) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                "INSERT INTO milestones (project_id, title, due_date, status, story_points) VALUES (?, ?, ?, ?, ?)"
            )
        );
        statement->setInt(1, milestone.projectId);
        statement->setString(2, milestone.title);
        statement->setString(3, milestone.dueDate);
        statement->setString(4, milestone.status);
        statement->setInt(5, milestone.storyPoints);
        statement->executeUpdate();

        std::unique_ptr<sql::PreparedStatement> idStatement(
            connection->prepareStatement("SELECT LAST_INSERT_ID()")
        );
        std::unique_ptr<sql::ResultSet> idResult(idStatement->executeQuery());
        idResult->next();
        return idResult->getInt(1);
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in milestone create: ") + sqlException.what());
    }
}

void MySQLMilestoneRepository::update(const Milestone& milestone) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                "UPDATE milestones SET title = ?, due_date = ?, status = ? WHERE milestone_id = ?"
            )
        );
        statement->setString(1, milestone.title);
        statement->setString(2, milestone.dueDate);
        statement->setString(3, milestone.status);
        statement->setInt(4, milestone.milestoneId);
        statement->executeUpdate();
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in milestone update: ") + sqlException.what());
    }
}

bool MySQLMilestoneRepository::milestoneBelongsToProject(int milestoneId, int projectId) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                "SELECT COUNT(*) FROM milestones WHERE milestone_id = ? AND project_id = ?"
            )
        );
        statement->setInt(1, milestoneId);
        statement->setInt(2, projectId);
        std::unique_ptr<sql::ResultSet> resultSet(statement->executeQuery());
        resultSet->next();
        return resultSet->getInt(1) > 0;
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in milestoneBelongsToProject: ") + sqlException.what());
    }
}
