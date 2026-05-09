#pragma once

#include "ISessionRepository.h"
#include "IDatabaseConnection.h"

class SQLiteSessionRepository : public ISessionRepository {
public:
    explicit SQLiteSessionRepository(IDatabaseConnection& db);
    void save(const Session& session) override;
    Session findByToken(const std::string& token) override;
    void deleteByToken(const std::string& token) override;
    void deleteByUserId(int userId) override;
private:
    IDatabaseConnection& db;
};
