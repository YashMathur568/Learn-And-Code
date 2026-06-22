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

        appConfig.llm.activeProvider = jsonConfig["llm"].value("activeProvider",
                                         jsonConfig["llm"].value("provider", std::string("groq")));
        appConfig.llm.geminiApiKey   = jsonConfig["llm"].value("geminiApiKey",  std::string(""));
        appConfig.llm.geminiModel    = jsonConfig["llm"].value("geminiModel",   std::string("gemini-2.0-flash-lite"));
        appConfig.llm.groqApiKey     = jsonConfig["llm"].value("groqApiKey",    jsonConfig["llm"].value("apiKey", std::string("")));
        appConfig.llm.groqModel      = jsonConfig["llm"].value("groqModel",     std::string("llama-3.3-70b-versatile"));
        appConfig.llm.gemmaHost      = jsonConfig["llm"].value("gemmaHost",     std::string(""));
        appConfig.llm.gemmaApiKey    = jsonConfig["llm"].value("gemmaApiKey",   jsonConfig["llm"].value("apiKey", std::string("")));
        appConfig.llm.gemmaModel     = jsonConfig["llm"].value("gemmaModel",    std::string(""));


        if (jsonConfig.contains("email")) {
            const auto& emailConfigSection = jsonConfig.at("email");
            appConfig.email.enabled     = emailConfigSection.value("enabled",     false);
            appConfig.email.smtpHost    = emailConfigSection.value("smtpHost",    std::string(""));
            appConfig.email.smtpPort    = emailConfigSection.value("smtpPort",    587);
            appConfig.email.username    = emailConfigSection.value("username",    std::string(""));
            appConfig.email.password    = emailConfigSection.value("password",    std::string(""));
            appConfig.email.fromAddress = emailConfigSection.value("fromAddress", std::string(""));
        }

        appConfig.schedulerIntervalHours = jsonConfig.at("schedulerIntervalHours").get<int>();
        appConfig.maxWeeklyHours         = jsonConfig.at("maxWeeklyHours").get<int>();
        appConfig.serverPort             = jsonConfig.at("serverPort").get<int>();
        appConfig.jwtSecret              = jsonConfig.at("jwtSecret").get<std::string>();

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

void ConfigLoader::setActiveProvider(const std::string& provider) {
    if (!loaded) throw AppException("Configuration not loaded.");
    appConfig.llm.activeProvider = provider;
}
