#pragma once

#include <string>
#include <variant>
#include <vector>

using ResultSet = std::vector<std::vector<std::string>>;
using SqlParam = std::variant<int, double, std::string>;

class IDatabaseConnection {
public:
    virtual ~IDatabaseConnection() = default;
    virtual void execute(const std::string& sqlStatement) = 0;
    virtual ResultSet query(const std::string& sqlStatement) = 0;
    virtual void executeParameterized(const std::string& sqlStatement, const std::vector<SqlParam>& params) = 0;
    virtual ResultSet queryParameterized(const std::string& sqlStatement, const std::vector<SqlParam>& params) = 0;
    virtual void close() = 0;
};
