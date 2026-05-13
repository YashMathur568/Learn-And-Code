#include "SQLiteUserRepository.h"

SQLiteUserRepository::SQLiteUserRepository(IDatabaseConnection& databaseConnection) : databaseConnection(databaseConnection) {}

void SQLiteUserRepository::save(const User& user)                              {}
User SQLiteUserRepository::findByUsername(const std::string& username)         { return {}; }
bool SQLiteUserRepository::existsByUsername(const std::string& username)       { return false; }
