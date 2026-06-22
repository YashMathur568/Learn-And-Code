#pragma once
#include "ISystemConfigRepository.hpp"

class MySQLSystemConfigRepository : public ISystemConfigRepository {
public:
    MySQLSystemConfigRepository() = default;

    std::map<std::string, std::string> getAllConfig() override;
    std::string getConfigValue(const std::string& key) override;
    bool updateConfigValue(const std::string& key, const std::string& value) override;
};
