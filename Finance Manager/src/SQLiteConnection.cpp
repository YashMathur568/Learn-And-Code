#include "SQLiteConnection.h"
#include <sqlite3.h>
#include <stdexcept>

SQLiteConnection::SQLiteConnection(const std::string& dbPath)
    : dbPath(dbPath), db(nullptr) {
    if (sqlite3_open(dbPath.c_str(), reinterpret_cast<sqlite3**>(&db)) != SQLITE_OK)
        throw std::runtime_error("Cannot open database: " + dbPath);
    initializeSchema();
}

SQLiteConnection::~SQLiteConnection() { close(); }

void SQLiteConnection::execute(const std::string& sql) {
    
}

ResultSet SQLiteConnection::query(const std::string& sql) {
    
    return {};
}

void SQLiteConnection::close() {
    if (db) {
        sqlite3_close(reinterpret_cast<sqlite3*>(db));
        db = nullptr;
    }
}

void SQLiteConnection::initializeSchema() {
    
}
