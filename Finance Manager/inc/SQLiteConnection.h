#pragma once

#include <string>
#include "IDatabaseConnection.h"

class SQLiteConnection : public IDatabaseConnection {
public:
    explicit SQLiteConnection(const std::string& dbPath);
    ~SQLiteConnection();
    void execute(const std::string& sqlStatement) override;
    ResultSet query(const std::string& sqlStatement) override;
    void executeParameterized(const std::string& sqlStatement, const std::vector<SqlParam>& params) override;
    ResultSet queryParameterized(const std::string& sqlStatement, const std::vector<SqlParam>& params) override;
    void close() override;
    void initializeSchema();
private:
    std::string databasePath;
    void* databaseHandle;
};