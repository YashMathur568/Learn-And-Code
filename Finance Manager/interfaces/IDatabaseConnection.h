#pragma once

#include <string>
#include <vector>

using ResultSet = std::vector<std::vector<std::string>>;

class IDatabaseConnection {
public:
    virtual ~IDatabaseConnection() = default;
    virtual void execute(const std::string& sql) = 0;
    virtual ResultSet query(const std::string& sql) = 0;
    virtual void close() = 0;
};
