#pragma once

#include <string>

struct DatabaseConfig {
    std::string host;
    int         port{3306};
    std::string name;
    std::string username;
    std::string password;
};

struct LlmConfig {
    std::string provider;
    std::string apiKey;
    std::string geminiModel;
    std::string groqModel;
};

struct AppConfig {
    DatabaseConfig database;
    LlmConfig      llm;
    int            schedulerIntervalHours{4};
    int            maxWeeklyHours{40};
    int            serverPort{8080};
};

class ConfigLoader {
public:
    static ConfigLoader& getInstance();

    void             load(const std::string& filePath);
    const AppConfig& getConfig() const;

private:
    ConfigLoader() = default;
    ConfigLoader(const ConfigLoader&)            = delete;
    ConfigLoader& operator=(const ConfigLoader&) = delete;

    AppConfig appConfig;
    bool      loaded{false};
};
