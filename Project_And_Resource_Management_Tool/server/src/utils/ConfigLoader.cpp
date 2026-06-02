#include "ConfigLoader.hpp"
#include "AppException.hpp"

#include <nlohmann/json.hpp>
#include <fstream>
#include <string>

ConfigLoader& ConfigLoader::getInstance() {
    static ConfigLoader instance;
    return instance;
}

void ConfigLoader::load(const std::string& filePath) {
    std::ifstream fileStream(filePath);
    if (!fileStream.is_open()) {
        throw AppException("Cannot open configuration file: " + filePath);
    }

    nlohmann::json jsonConfig;

    try {
        fileStream >> jsonConfig;

        appConfig.database.host     = jsonConfig.at("database").at("host").get<std::string>();
        appConfig.database.port     = jsonConfig.at("database").at("port").get<int>();
        appConfig.database.name     = jsonConfig.at("database").at("name").get<std::string>();
        appConfig.database.username = jsonConfig.at("database").at("username").get<std::string>();
        appConfig.database.password = jsonConfig.at("database").at("password").get<std::string>();

        appConfig.llm.provider    = jsonConfig.at("llm").at("provider").get<std::string>();
        appConfig.llm.apiKey      = jsonConfig.at("llm").at("apiKey").get<std::string>();
        appConfig.llm.geminiModel = jsonConfig.at("llm").at("geminiModel").get<std::string>();
        appConfig.llm.groqModel   = jsonConfig.at("llm").at("groqModel").get<std::string>();

        appConfig.schedulerIntervalHours = jsonConfig.at("schedulerIntervalHours").get<int>();
        appConfig.maxWeeklyHours         = jsonConfig.at("maxWeeklyHours").get<int>();
        appConfig.serverPort             = jsonConfig.at("serverPort").get<int>();

    } catch (const nlohmann::json::parse_error& parseError) {
        throw AppException(std::string("Configuration file parse error: ") + parseError.what());
    } catch (const nlohmann::json::out_of_range& rangeError) {
        throw AppException(std::string("Missing required configuration key: ") + rangeError.what());
    } catch (const nlohmann::json::type_error& typeError) {
        throw AppException(std::string("Configuration value type mismatch: ") + typeError.what());
    }

    loaded = true;
}

const AppConfig& ConfigLoader::getConfig() const {
    if (!loaded) {
        throw AppException("Configuration not loaded. Call load() before getConfig().");
    }
    return appConfig;
}
