#include "MySQLSystemConfigRepository.hpp"
#include "DatabasePool.hpp"
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/exception.h>
#include <iostream>

std::map<std::string, std::string> MySQLSystemConfigRepository::getAllConfig() {
    std::map<std::string, std::string> config;

    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement("SELECT config_key, config_value FROM system_config")
        );
        std::unique_ptr<sql::ResultSet> resultSet(statement->executeQuery());

        while (resultSet->next()) {
            std::string key = std::string(resultSet->getString("config_key"));
            std::string value = std::string(resultSet->getString("config_value"));
            config[key] = value;
        }
    } catch (const sql::SQLException& sqlException) {
        std::cerr << "[ERROR] Failed to get all config: " << sqlException.what() << std::endl;
    }

    return config;
}

std::string MySQLSystemConfigRepository::getConfigValue(const std::string& key) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement("SELECT config_value FROM system_config WHERE config_key = ?")
        );
        statement->setString(1, key);
        std::unique_ptr<sql::ResultSet> resultSet(statement->executeQuery());

        if (resultSet->next()) {
            return std::string(resultSet->getString("config_value"));
        }
    } catch (const sql::SQLException& sqlException) {
        std::cerr << "[ERROR] Failed to get config value for key '" << key << "': " << sqlException.what() << std::endl;
    }

    return "";
}

bool MySQLSystemConfigRepository::updateConfigValue(const std::string& key, const std::string& value) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement("UPDATE system_config SET config_value = ? WHERE config_key = ?")
        );
        statement->setString(1, value);
        statement->setString(2, key);
        statement->executeUpdate();
        return true;
    } catch (const sql::SQLException& sqlException) {
        std::cerr << "[ERROR] Failed to update config value for key '" << key << "': " << sqlException.what() << std::endl;
        return false;
    }
}
