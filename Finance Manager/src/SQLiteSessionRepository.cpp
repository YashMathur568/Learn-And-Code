#include "SQLiteSessionRepository.h"
#include <stdexcept>

SQLiteSessionRepository::SQLiteSessionRepository(IDatabaseConnection& databaseConnection) : databaseConnection(databaseConnection) {}

void SQLiteSessionRepository::save(const Session& session) {
    databaseConnection.executeParameterized(
        "INSERT INTO sessions (user_id, token, expires_at) VALUES (?, ?, ?);",
        { session.userId, session.token, session.expiresAt }
    );
}

Session SQLiteSessionRepository::findByToken(const std::string& token) {
    ResultSet rows = databaseConnection.queryParameterized(
        "SELECT id, user_id, token, expires_at FROM sessions WHERE token = ?;",
        { token }
    );
    if (rows.empty())
        throw std::runtime_error("Session not found");
    const auto& row = rows[0];
    Session session;
    session.id        = std::stoi(row[0]);
    session.userId    = std::stoi(row[1]);
    session.token     = row[2];
    session.expiresAt = row[3];
    return session;
}

void SQLiteSessionRepository::deleteByToken(const std::string& token) {
    databaseConnection.executeParameterized(
        "DELETE FROM sessions WHERE token = ?;",
        { token }
    );
}

void SQLiteSessionRepository::deleteByUserId(int userId) {
    databaseConnection.executeParameterized(
        "DELETE FROM sessions WHERE user_id = ?;",
        { userId }
    );
}
