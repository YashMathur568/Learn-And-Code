#include "MySQLUserRepository.hpp"
#include "../utils/AppException.hpp"

#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/exception.h>

#include <memory>
#include <optional>
#include <vector>

User MySQLUserRepository::mapRowToUser(sql::ResultSet* resultSet) {
    User user;
    user.userId         = resultSet->getInt("user_id");
    user.fullName       = resultSet->getString("full_name").asStdString();
    user.email          = resultSet->getString("email").asStdString();
    user.username       = resultSet->getString("username").asStdString();
    user.passwordHash   = resultSet->getString("password_hash").asStdString();
    user.role           = resultSet->getString("role").asStdString();
    user.isActive       = resultSet->getBoolean("is_active");
    user.forcePwdChange = resultSet->getBoolean("force_pwd_change");
    user.createdAt      = resultSet->getString("created_at").asStdString();
    return user;
}

std::optional<User> MySQLUserRepository::findByUsername(const std::string& username) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                "SELECT user_id, full_name, email, username, password_hash, "
                "role, is_active, force_pwd_change, created_at "
                "FROM users WHERE BINARY username = ?"
            )
        );
        statement->setString(1, username);
        std::unique_ptr<sql::ResultSet> resultSet(statement->executeQuery());
        if (resultSet->next()) {
            return mapRowToUser(resultSet.get());
        }
        return std::nullopt;
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("Database error in findByUsername: ") + sqlException.what());
    }
}

std::optional<User> MySQLUserRepository::findById(int userId) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                "SELECT user_id, full_name, email, username, password_hash, "
                "role, is_active, force_pwd_change, created_at "
                "FROM users WHERE user_id = ?"
            )
        );
        statement->setInt(1, userId);
        std::unique_ptr<sql::ResultSet> resultSet(statement->executeQuery());
        if (resultSet->next()) {
            return mapRowToUser(resultSet.get());
        }
        return std::nullopt;
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("Database error in findById: ") + sqlException.what());
    }
}

std::vector<User> MySQLUserRepository::findAll() {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                "SELECT user_id, full_name, email, username, password_hash, "
                "role, is_active, force_pwd_change, created_at "
                "FROM users ORDER BY user_id"
            )
        );
        std::unique_ptr<sql::ResultSet> resultSet(statement->executeQuery());
        std::vector<User> users;
        while (resultSet->next()) {
            users.push_back(mapRowToUser(resultSet.get()));
        }
        return users;
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("Database error in findAll: ") + sqlException.what());
    }
}

int MySQLUserRepository::create(const User& user) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                "INSERT INTO users (full_name, email, username, password_hash, role, "
                "is_active, force_pwd_change) VALUES (?, ?, ?, ?, ?, ?, ?)"
            )
        );
        statement->setString(1, user.fullName);
        statement->setString(2, user.email);
        statement->setString(3, user.username);
        statement->setString(4, user.passwordHash);
        statement->setString(5, user.role);
        statement->setBoolean(6, user.isActive);
        statement->setBoolean(7, user.forcePwdChange);
        statement->executeUpdate();

        std::unique_ptr<sql::PreparedStatement> idStatement(
            connection->prepareStatement("SELECT LAST_INSERT_ID()")
        );
        std::unique_ptr<sql::ResultSet> idResult(idStatement->executeQuery());
        idResult->next();
        return idResult->getInt(1);
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("Database error in create user: ") + sqlException.what());
    }
}

void MySQLUserRepository::updatePasswordHash(int userId, const std::string& newHash) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                "UPDATE users SET password_hash = ? WHERE user_id = ?"
            )
        );
        statement->setString(1, newHash);
        statement->setInt(2, userId);
        statement->executeUpdate();
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("Database error in updatePasswordHash: ") + sqlException.what());
    }
}

void MySQLUserRepository::setForcePwdChange(int userId, bool value) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                "UPDATE users SET force_pwd_change = ? WHERE user_id = ?"
            )
        );
        statement->setBoolean(1, value);
        statement->setInt(2, userId);
        statement->executeUpdate();
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("Database error in setForcePwdChange: ") + sqlException.what());
    }
}

void MySQLUserRepository::setActiveStatus(int userId, bool active) {
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
        throw AppException(std::string("Database error in setActiveStatus: ") + sqlException.what());
    }
}

bool MySQLUserRepository::existsByUsername(const std::string& username) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                "SELECT COUNT(*) FROM users WHERE BINARY username = ?"
            )
        );
        statement->setString(1, username);
        std::unique_ptr<sql::ResultSet> resultSet(statement->executeQuery());
        resultSet->next();
        return resultSet->getInt(1) > 0;
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("Database error in existsByUsername: ") + sqlException.what());
    }
}

bool MySQLUserRepository::existsByEmail(const std::string& email) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                "SELECT COUNT(*) FROM users WHERE email = ?"
            )
        );
        statement->setString(1, email);
        std::unique_ptr<sql::ResultSet> resultSet(statement->executeQuery());
        resultSet->next();
        return resultSet->getInt(1) > 0;
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("Database error in existsByEmail: ") + sqlException.what());
    }
}
