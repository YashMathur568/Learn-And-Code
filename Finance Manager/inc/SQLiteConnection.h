#pragma once

#include <string>
#include "IDatabaseConnection.h"

class SQLiteConnection : public IDatabaseConnection {
public:
    explicit SQLiteConnection(const std::string& dbPath);
    ~SQLiteConnection();
    void execute(const std::string& sql) override;
    ResultSet query(const std::string& sql) override;
    void close() override;
    void initializeSchema();
private:
    std::string dbPath;
    void* db;   
};
