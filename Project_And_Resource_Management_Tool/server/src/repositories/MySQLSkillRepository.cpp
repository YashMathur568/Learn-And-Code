#include "MySQLSkillRepository.hpp"
#include "../utils/AppException.hpp"

#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/exception.h>

#include <memory>

EmployeeSkill MySQLSkillRepository::mapRowToSkill(sql::ResultSet* resultSet) {
    EmployeeSkill skill;
    skill.skillId    = resultSet->getInt("skill_id");
    skill.employeeId = resultSet->getInt("employee_id");
    skill.skillName  = resultSet->getString("skill_name").asStdString();
    skill.category   = resultSet->getString("category").asStdString();
    skill.proficiency= resultSet->getString("proficiency").asStdString();
    return skill;
}

std::vector<EmployeeSkill> MySQLSkillRepository::findByEmployeeId(int employeeId) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                "SELECT skill_id, employee_id, skill_name, category, proficiency "
                "FROM employee_skills WHERE employee_id = ? ORDER BY skill_id"
            )
        );
        statement->setInt(1, employeeId);
        std::unique_ptr<sql::ResultSet> resultSet(statement->executeQuery());
        std::vector<EmployeeSkill> skills;
        while (resultSet->next()) {
            skills.push_back(mapRowToSkill(resultSet.get()));
        }
        return skills;
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in findSkillsByEmployee: ") + sqlException.what());
    }
}

std::optional<EmployeeSkill> MySQLSkillRepository::findById(int skillId) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                "SELECT skill_id, employee_id, skill_name, category, proficiency "
                "FROM employee_skills WHERE skill_id = ?"
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
                "INSERT INTO employee_skills (employee_id, skill_name, category, proficiency) "
                "VALUES (?, ?, ?, ?)"
            )
        );
        statement->setInt(1, skill.employeeId);
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
                "UPDATE employee_skills SET skill_name = ?, category = ?, proficiency = ? "
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
                "DELETE FROM employee_skills WHERE skill_id = ?"
            )
        );
        statement->setInt(1, skillId);
        statement->executeUpdate();
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in skill remove: ") + sqlException.what());
    }
}

bool MySQLSkillRepository::skillBelongsToEmployee(int skillId, int employeeId) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                "SELECT COUNT(*) FROM employee_skills WHERE skill_id = ? AND employee_id = ?"
            )
        );
        statement->setInt(1, skillId);
        statement->setInt(2, employeeId);
        std::unique_ptr<sql::ResultSet> resultSet(statement->executeQuery());
        resultSet->next();
        return resultSet->getInt(1) > 0;
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in skillBelongsToEmployee: ") + sqlException.what());
    }
}
