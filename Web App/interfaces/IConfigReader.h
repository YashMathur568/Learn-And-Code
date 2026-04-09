#pragma once
#include <string>

class IConfigReader {
public:
    virtual ~IConfigReader() = default;
    virtual std::string readValue(const std::string& key) = 0;
};
