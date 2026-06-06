#pragma once

#include <string>

class SqlValidator {
public:
    static void validate(const std::string& sql);

private:
    static std::string normalize(const std::string& sql);
    static bool containsBlockedKeyword(const std::string& normalizedSql);
    static bool isSelectStatement(const std::string& normalizedSql);
    static bool hasStackedQueries(const std::string& sql);
    static bool accessesAllowedTablesOnly(const std::string& normalizedSql);
};
