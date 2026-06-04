#include "MySQLConfigRepository.hpp"
#include "../utils/AppException.hpp"

#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/exception.h>

#include <memory>

SystemConfig MySQLConfigRepository::mapRowToConfig(sql::ResultSet* resultSet) {
    SystemConfig config;
    config.configKey   = resultSet->getString("config_key").asStdString();
    config.configValue = resultSet->getString("config_value").asStdString();
    return config;
}

std::optional<SystemConfig> MySQLConfigRepository::findByKey(const std::string& key) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                "SELECT config_key, config_value FROM system_config WHERE config_key = ?"
            )
        );
        statement->setString(1, key);
        std::unique_ptr<sql::ResultSet> resultSet(statement->executeQuery());
        if (resultSet->next()) {
            return mapRowToConfig(resultSet.get());
        }
        return std::nullopt;
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in config findByKey: ") + sqlException.what());
    }
}

std::vector<SystemConfig> MySQLConfigRepository::findAll() {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                "SELECT config_key, config_value FROM system_config ORDER BY config_key"
            )
        );
        std::unique_ptr<sql::ResultSet> resultSet(statement->executeQuery());
        std::vector<SystemConfig> configs;
        while (resultSet->next()) {
            configs.push_back(mapRowToConfig(resultSet.get()));
        }
        return configs;
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in config findAll: ") + sqlException.what());
    }
}

void MySQLConfigRepository::upsert(const std::string& key, const std::string& value) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                "INSERT INTO system_config (config_key, config_value) VALUES (?, ?) "
                "ON DUPLICATE KEY UPDATE config_value = VALUES(config_value)"
            )
        );
        statement->setString(1, key);
        statement->setString(2, value);
        statement->executeUpdate();
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in config upsert: ") + sqlException.what());
    }
}
