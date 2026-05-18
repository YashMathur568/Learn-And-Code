#include "SQLiteUserRepository.h"
#include <stdexcept>

static std::string escapeSqlString(const std::string& value) {
    std::string result;
    for (char character : value) {
        if (character == '\'')
            result += "''";
        else
            result += character;
    }
    return result;
}

SQLiteUserRepository::SQLiteUserRepository(IDatabaseConnection& databaseConnection) : databaseConnection(databaseConnection) {}

void SQLiteUserRepository::save(const User& user) {
    std::string sqlStatement =
        "INSERT INTO users (username, password_hash, salt, created_at) VALUES ('"
        + escapeSqlString(user.username) + "', '"
        + user.passwordHash + "', '"
        + user.salt + "', '"
        + user.createdAt + "');";
    databaseConnection.execute(sqlStatement);
}

User SQLiteUserRepository::findByUsername(const std::string& username) {
    std::string sqlStatement =
        "SELECT id, username, password_hash, salt, created_at FROM users WHERE username = '"
        + escapeSqlString(username) + "';";
    ResultSet rows = databaseConnection.query(sqlStatement);
    if (rows.empty())
        throw std::runtime_error("User not found: " + username);
    const auto& row = rows[0];
    User user;
    user.id           = std::stoi(row[0]);
    user.username     = row[1];
    user.passwordHash = row[2];
    user.salt         = row[3];
    user.createdAt    = row[4];
    return user;
}

bool SQLiteUserRepository::existsByUsername(const std::string& username) {
    std::string sqlStatement =
        "SELECT COUNT(*) FROM users WHERE username = '"
        + escapeSqlString(username) + "';";
    ResultSet rows = databaseConnection.query(sqlStatement);
    return !rows.empty() && rows[0][0] != "0";
}