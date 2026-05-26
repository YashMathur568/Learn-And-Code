#include "SQLiteUserRepository.h"
#include <stdexcept>

SQLiteUserRepository::SQLiteUserRepository(IDatabaseConnection& databaseConnection) : databaseConnection(databaseConnection) {}

void SQLiteUserRepository::save(const User& user) {
    databaseConnection.executeParameterized(
        "INSERT INTO users (username, password_hash, salt, created_at) VALUES (?, ?, ?, ?);",
        { user.username, user.passwordHash, user.salt, user.createdAt }
    );
}

User SQLiteUserRepository::findByUsername(const std::string& username) {
    ResultSet rows = databaseConnection.queryParameterized(
        "SELECT id, username, password_hash, salt, created_at FROM users WHERE username = ?;",
        { username }
    );
    if (rows.empty())
        throw std::runtime_error("User not found: " + username);
    const auto& row = rows[0];
    User user;
    user.userId       = std::stoi(row[0]);
    user.username     = row[1];
    user.passwordHash = row[2];
    user.salt         = row[3];
    user.createdAt    = row[4];
    return user;
}

bool SQLiteUserRepository::existsByUsername(const std::string& username) {
    ResultSet rows = databaseConnection.queryParameterized(
        "SELECT COUNT(*) FROM users WHERE username = ?;",
        { username }
    );
    return !rows.empty() && rows[0][0] != "0";
}
