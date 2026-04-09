#pragma once
#include "../interfaces/IConfigReader.h"
#include <string>
#include <unordered_map>

class ConfigReader : public IConfigReader {
private:
    std::string configFilePath;
    std::unordered_map<std::string, std::string> configValues;

    void loadConfig();

public:
    explicit ConfigReader(const std::string& filePath);
    std::string readValue(const std::string& key) override;
};
