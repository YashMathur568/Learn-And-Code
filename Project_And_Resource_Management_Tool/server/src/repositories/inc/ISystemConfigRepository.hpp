#pragma once
#include <string>
#include <map>
#include <memory>

class ISystemConfigRepository {
public:
    virtual ~ISystemConfigRepository() = default;


    virtual std::map<std::string, std::string> getAllConfig() = 0;


    virtual std::string getConfigValue(const std::string& key) = 0;


    virtual bool updateConfigValue(const std::string& key, const std::string& value) = 0;
};
