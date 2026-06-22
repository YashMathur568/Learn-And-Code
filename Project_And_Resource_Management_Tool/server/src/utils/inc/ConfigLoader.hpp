#pragma once

#include <string>

struct DatabaseConfig {
    std::string host;
    int         port{3306};
    std::string name;
    std::string username;
    std::string password;
};

struct EmailConfig {
    bool        enabled{false};
    std::string smtpHost;
    int         smtpPort{587};
    std::string username;
    std::string password;
    std::string fromAddress;
};

struct LlmConfig {
    std::string activeProvider;
    std::string geminiApiKey;
    std::string geminiModel;
    std::string groqApiKey;
    std::string groqModel;
    std::string gemmaHost;
    std::string gemmaApiKey;
    std::string gemmaModel;
};

struct AppConfig {
    DatabaseConfig database;
    LlmConfig      llm;
    EmailConfig    email;
    int            schedulerIntervalHours{4};
    int            maxWeeklyHours{40};
    int            serverPort{8080};
    std::string    jwtSecret;
};

class ConfigLoader {
public:
    static ConfigLoader& getInstance();

    void             load(const std::string& filePath);
    const AppConfig& getConfig() const;
    void             setActiveProvider(const std::string& provider);

private:
    ConfigLoader() = default;
    ConfigLoader(const ConfigLoader&)            = delete;
    ConfigLoader& operator=(const ConfigLoader&) = delete;

    AppConfig appConfig;
    bool      loaded{false};
};
