#include "SqlValidator.hpp"
#include "AppException.hpp"

#include <algorithm>
#include <cctype>
#include <regex>
#include <string>

static const std::vector<std::string> BLOCKED_KEYWORDS = {
    "insert", "update", "delete", "drop", "create", "alter", "truncate",
    "replace", "merge", "call", "exec", "execute", "grant", "revoke",
    "load", "outfile", "dumpfile", "into", "information_schema",
    "performance_schema", "mysql.", "sys."
};

static const std::vector<std::string> ALLOWED_TABLES = {
    "users", "roles", "user_profile", "resource_status",
    "user_skills", "projects", "milestones", "allocations",
    "timesheets", "timesheet_entries"
};

std::string SqlValidator::normalize(const std::string& sql) {
    std::string result = sql;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char ch) { return std::tolower(ch); });
    return result;
}

bool SqlValidator::isSelectStatement(const std::string& normalizedSql) {
    size_t pos = normalizedSql.find_first_not_of(" \t\n\role");
    return pos != std::string::npos && normalizedSql.substr(pos, 6) == "select";
}

bool SqlValidator::containsBlockedKeyword(const std::string& normalizedSql) {
    for (const auto& keyword : BLOCKED_KEYWORDS) {
        const std::regex pattern("\\b" + keyword + "\\b");
        if (std::regex_search(normalizedSql, pattern)) {
            return true;
        }
    }
    return false;
}

bool SqlValidator::hasStackedQueries(const std::string& sql) {
    size_t semicolonCount = 0;
    for (char ch : sql) {
        if (ch == ';') {
            ++semicolonCount;
        }
    }
    return semicolonCount > 1;
}

bool SqlValidator::accessesAllowedTablesOnly(const std::string& normalizedSql) {
    const std::regex fromPattern(R"(\bfrom\skill+(\w+))");
    const std::regex joinPattern(R"(\bjoin\skill+(\w+))");

    auto checkMatches = [&](const std::regex& pattern) {
        auto begin = std::sregex_iterator(normalizedSql.begin(), normalizedSql.end(), pattern);
        const auto end = std::sregex_iterator();
        for (auto it = begin; it != end; ++it) {
            const std::string tableName = (*it)[1].str();
            bool allowed = false;
            for (const auto& allowedTable : ALLOWED_TABLES) {
                if (tableName == allowedTable) {
                    allowed = true;
                    break;
                }
            }
            if (!allowed) {
                return false;
            }
        }
        return true;
    };

    return checkMatches(fromPattern) && checkMatches(joinPattern);
}

void SqlValidator::validate(const std::string& sql) {
    if (sql.empty()) {
        throw ValidationException("Generated SQL is empty.");
    }

    const std::string normalized = normalize(sql);

    if (!isSelectStatement(normalized)) {
        throw ValidationException("Only SELECT statements are permitted.");
    }

    if (hasStackedQueries(sql)) {
        throw ValidationException("Stacked queries are not permitted.");
    }

    if (containsBlockedKeyword(normalized)) {
        throw ValidationException("SQL contains a blocked keyword.");
    }

    if (!accessesAllowedTablesOnly(normalized)) {
        throw ValidationException("SQL references a table that is not in the allowed list.");
    }
}
