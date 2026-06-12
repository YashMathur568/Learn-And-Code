#include "MySQLSkillRepository.hpp"
#include "../utils/AppException.hpp"

#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/exception.h>

#include <memory>

EmployeeSkill MySQLSkillRepository::mapRowToSkill(sql::ResultSet* resultSet) {
    EmployeeSkill skill;
    skill.skillId    = resultSet->getInt("skill_id");
    skill.userId     = resultSet->getInt("user_id");
    skill.skillName  = resultSet->getString("skill_name");
    skill.category   = resultSet->getString("category");
    skill.proficiency= resultSet->getString("proficiency");
    return skill;
}

std::vector<EmployeeSkill> MySQLSkillRepository::findByUserId(int userId) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                "SELECT skill_id, user_id, skill_name, category, proficiency "
                "FROM user_skills WHERE user_id = ? ORDER BY skill_id"
            )
        );
        statement->setInt(1, userId);
        std::unique_ptr<sql::ResultSet> resultSet(statement->executeQuery());
        std::vector<EmployeeSkill> skills;
        while (resultSet->next()) {
            skills.push_back(mapRowToSkill(resultSet.get()));
        }
        return skills;
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in findSkillsByUser: ") + sqlException.what());
    }
}

std::optional<EmployeeSkill> MySQLSkillRepository::findById(int skillId) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                "SELECT skill_id, user_id, skill_name, category, proficiency "
                "FROM user_skills WHERE skill_id = ?"
            )
        );
        statement->setInt(1, skillId);
        std::unique_ptr<sql::ResultSet> resultSet(statement->executeQuery());
        if (resultSet->next()) {
            return mapRowToSkill(resultSet.get());
        }
        return std::nullopt;
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in skill findById: ") + sqlException.what());
    }
}

int MySQLSkillRepository::create(const EmployeeSkill& skill) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                "INSERT INTO user_skills (user_id, skill_name, category, proficiency) "
                "VALUES (?, ?, ?, ?)"
            )
        );
        statement->setInt(1, skill.userId);
        statement->setString(2, skill.skillName);
        statement->setString(3, skill.category);
        statement->setString(4, skill.proficiency);
        statement->executeUpdate();

        std::unique_ptr<sql::PreparedStatement> idStatement(
            connection->prepareStatement("SELECT LAST_INSERT_ID()")
        );
        std::unique_ptr<sql::ResultSet> idResult(idStatement->executeQuery());
        idResult->next();
        return idResult->getInt(1);
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in skill create: ") + sqlException.what());
    }
}

void MySQLSkillRepository::update(const EmployeeSkill& skill) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                "UPDATE user_skills SET skill_name = ?, category = ?, proficiency = ? "
                "WHERE skill_id = ?"
            )
        );
        statement->setString(1, skill.skillName);
        statement->setString(2, skill.category);
        statement->setString(3, skill.proficiency);
        statement->setInt(4, skill.skillId);
        statement->executeUpdate();
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in skill update: ") + sqlException.what());
    }
}

void MySQLSkillRepository::remove(int skillId) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                "DELETE FROM user_skills WHERE skill_id = ?"
            )
        );
        statement->setInt(1, skillId);
        statement->executeUpdate();
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in skill remove: ") + sqlException.what());
    }
}

bool MySQLSkillRepository::skillBelongsToUser(int skillId, int userId) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                "SELECT COUNT(*) FROM user_skills WHERE skill_id = ? AND user_id = ?"
            )
        );
        statement->setInt(1, skillId);
        statement->setInt(2, userId);
        std::unique_ptr<sql::ResultSet> resultSet(statement->executeQuery());
        resultSet->next();
        return resultSet->getInt(1) > 0;
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in skillBelongsToUser: ") + sqlException.what());
    }
}

