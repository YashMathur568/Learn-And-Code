#pragma once

#include "IBudgetRepository.h"
#include "IDatabaseConnection.h"

class SQLiteBudgetRepository : public IBudgetRepository {
public:
    explicit SQLiteBudgetRepository(IDatabaseConnection& databaseConnection);
    void save(const Budget& budget) override;
    Budget findByCategory(int userId, Category category, const std::string& month) override;
    std::vector<Budget> findAll(int userId) override;
private:
    IDatabaseConnection& databaseConnection;
};
