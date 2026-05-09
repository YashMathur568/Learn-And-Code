#pragma once

#include "IUserRepository.h"
#include "IDatabaseConnection.h"

class SQLiteUserRepository : public IUserRepository {
public:
    explicit SQLiteUserRepository(IDatabaseConnection& db);
    void save(const User& user) override;
    User findByUsername(const std::string& username) override;
    bool existsByUsername(const std::string& username) override;
private:
    IDatabaseConnection& db;
};
