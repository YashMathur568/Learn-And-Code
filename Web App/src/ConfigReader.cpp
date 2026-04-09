#include "../inc/ConfigReader.h"
#include <fstream>
#include <stdexcept>

ConfigReader::ConfigReader(const std::string& filePath)
    : configFilePath(filePath) {
    loadConfig();
}

void ConfigReader::loadConfig() {
    std::ifstream fileStream(configFilePath);
    if (!fileStream.is_open()) {
        throw std::runtime_error("Cannot open config file: " + configFilePath);
    }

    std::string line;
    while (std::getline(fileStream, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }

        size_t delimiterPosition = line.find('=');
        if (delimiterPosition == std::string::npos) {
            continue;
        }

        std::string key = line.substr(0, delimiterPosition);
        std::string value = line.substr(delimiterPosition + 1);

        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);

        if (!key.empty()) {
            configValues[key] = value;
        }
    }
}

std::string ConfigReader::readValue(const std::string& key) {
    auto iterator = configValues.find(key);
    if (iterator == configValues.end()) {
        throw std::runtime_error("Config key not found: " + key);
    }
    return iterator->second;
}
