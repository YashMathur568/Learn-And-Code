#include "SQLiteSessionRepository.h"
#include <stdexcept>

SQLiteSessionRepository::SQLiteSessionRepository(IDatabaseConnection& databaseConnection) : databaseConnection(databaseConnection) {}

void SQLiteSessionRepository::save(const Session& session) {
    std::string sqlStatement =
        "INSERT INTO sessions (user_id, token, expires_at) VALUES ("
        + std::to_string(session.userId) + ", '"
        + session.token + "', '"
        + session.expiresAt + "');";
    databaseConnection.execute(sqlStatement);
}

Session SQLiteSessionRepository::findByToken(const std::string& token) {
    std::string sqlStatement =
        "SELECT id, user_id, token, expires_at FROM sessions WHERE token = '"
        + token + "';";
    ResultSet rows = databaseConnection.query(sqlStatement);
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
    std::string sqlStatement = "DELETE FROM sessions WHERE token = '" + token + "';";
    databaseConnection.execute(sqlStatement);
}

void SQLiteSessionRepository::deleteByUserId(int userId) {
    std::string sqlStatement = "DELETE FROM sessions WHERE user_id = " + std::to_string(userId) + ";";
    databaseConnection.execute(sqlStatement);
}
