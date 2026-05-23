#include "SQLiteConnection.h"
#include <sqlite3.h>
#include <stdexcept>

SQLiteConnection::SQLiteConnection(const std::string& databasePath)
    : databasePath(databasePath), databaseHandle(nullptr) {
    if (sqlite3_open(databasePath.c_str(), reinterpret_cast<sqlite3**>(&databaseHandle)) != SQLITE_OK)
        throw std::runtime_error("Cannot open database: " + databasePath);
    initializeSchema();
}

SQLiteConnection::~SQLiteConnection() { close(); }

void SQLiteConnection::execute(const std::string& sqlStatement) {
    char* errorMessage = nullptr;
    int resultCode = sqlite3_exec(reinterpret_cast<sqlite3*>(databaseHandle), sqlStatement.c_str(), nullptr, nullptr, &errorMessage);
    if (resultCode != SQLITE_OK) {
        std::string errorText = errorMessage ? errorMessage : "Unknown error";
        sqlite3_free(errorMessage);
        throw std::runtime_error("SQL execute error: " + errorText);
    }
}

ResultSet SQLiteConnection::query(const std::string& sqlStatement) {
    ResultSet results;
    sqlite3_stmt* preparedStatement = nullptr;
    int resultCode = sqlite3_prepare_v2(reinterpret_cast<sqlite3*>(databaseHandle), sqlStatement.c_str(), -1, &preparedStatement, nullptr);
    if (resultCode != SQLITE_OK)
        throw std::runtime_error("SQL prepare error: " + std::string(sqlite3_errmsg(reinterpret_cast<sqlite3*>(databaseHandle))));
    while (sqlite3_step(preparedStatement) == SQLITE_ROW) {
        std::vector<std::string> row;
        int columnCount = sqlite3_column_count(preparedStatement);
        for (int columnIndex = 0; columnIndex < columnCount; ++columnIndex) {
            const unsigned char* columnValue = sqlite3_column_text(preparedStatement, columnIndex);
            row.push_back(columnValue ? reinterpret_cast<const char*>(columnValue) : "");
        }
        results.push_back(row);
    }
    sqlite3_finalize(preparedStatement);
    return results;
}

static void bindParams(sqlite3_stmt* statement, const std::vector<SqlParam>& params) {
    for (int paramIndex = 0; paramIndex < static_cast<int>(params.size()); ++paramIndex) {
        std::visit([&](auto&& value) {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, int>)
                sqlite3_bind_int(statement, paramIndex + 1, value);
            else if constexpr (std::is_same_v<T, double>)
                sqlite3_bind_double(statement, paramIndex + 1, value);
            else
                sqlite3_bind_text(statement, paramIndex + 1, value.c_str(), -1, SQLITE_TRANSIENT);
        }, params[paramIndex]);
    }
}

void SQLiteConnection::executeParameterized(const std::string& sqlStatement, const std::vector<SqlParam>& params) {
    sqlite3_stmt* preparedStatement = nullptr;
    int resultCode = sqlite3_prepare_v2(reinterpret_cast<sqlite3*>(databaseHandle), sqlStatement.c_str(), -1, &preparedStatement, nullptr);
    if (resultCode != SQLITE_OK)
        throw std::runtime_error("SQL prepare error: " + std::string(sqlite3_errmsg(reinterpret_cast<sqlite3*>(databaseHandle))));
    bindParams(preparedStatement, params);
    resultCode = sqlite3_step(preparedStatement);
    sqlite3_finalize(preparedStatement);
    if (resultCode != SQLITE_DONE)
        throw std::runtime_error("SQL execute error: " + std::string(sqlite3_errmsg(reinterpret_cast<sqlite3*>(databaseHandle))));
}

ResultSet SQLiteConnection::queryParameterized(const std::string& sqlStatement, const std::vector<SqlParam>& params) {
    ResultSet results;
    sqlite3_stmt* preparedStatement = nullptr;
    int resultCode = sqlite3_prepare_v2(reinterpret_cast<sqlite3*>(databaseHandle), sqlStatement.c_str(), -1, &preparedStatement, nullptr);
    if (resultCode != SQLITE_OK)
        throw std::runtime_error("SQL prepare error: " + std::string(sqlite3_errmsg(reinterpret_cast<sqlite3*>(databaseHandle))));
    bindParams(preparedStatement, params);
    while (sqlite3_step(preparedStatement) == SQLITE_ROW) {
        std::vector<std::string> row;
        int columnCount = sqlite3_column_count(preparedStatement);
        for (int columnIndex = 0; columnIndex < columnCount; ++columnIndex) {
            const unsigned char* columnValue = sqlite3_column_text(preparedStatement, columnIndex);
            row.push_back(columnValue ? reinterpret_cast<const char*>(columnValue) : "");
        }
        results.push_back(row);
    }
    sqlite3_finalize(preparedStatement);
    return results;
}

void SQLiteConnection::close() {
    if (databaseHandle) {
        sqlite3_close(reinterpret_cast<sqlite3*>(databaseHandle));
        databaseHandle = nullptr;
    }
}

void SQLiteConnection::initializeSchema() {
    execute(
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "username TEXT NOT NULL UNIQUE,"
        "password_hash TEXT NOT NULL,"
        "salt TEXT NOT NULL,"
        "created_at TEXT NOT NULL"
        ");"
    );
    execute(
        "CREATE TABLE IF NOT EXISTS sessions ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "user_id INTEGER NOT NULL,"
        "token TEXT NOT NULL UNIQUE,"
        "expires_at TEXT NOT NULL,"
        "FOREIGN KEY(user_id) REFERENCES users(id)"
        ");"
    );
    execute(
        "CREATE TABLE IF NOT EXISTS expenses ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "user_id INTEGER NOT NULL,"
        "amount REAL NOT NULL,"
        "category TEXT NOT NULL,"
        "description TEXT,"
        "date TEXT NOT NULL,"
        "FOREIGN KEY(user_id) REFERENCES users(id)"
        ");"
    );
    execute(
        "CREATE TABLE IF NOT EXISTS income ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "user_id INTEGER NOT NULL,"
        "amount REAL NOT NULL,"
        "source TEXT NOT NULL,"
        "description TEXT,"
        "date TEXT NOT NULL,"
        "FOREIGN KEY(user_id) REFERENCES users(id)"
        ");"
    );
    execute(
        "CREATE TABLE IF NOT EXISTS budgets ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "user_id INTEGER NOT NULL,"
        "category TEXT NOT NULL,"
        "limit_amount REAL NOT NULL,"
        "month TEXT NOT NULL,"
        "FOREIGN KEY(user_id) REFERENCES users(id)"
        ");"
    );
}
