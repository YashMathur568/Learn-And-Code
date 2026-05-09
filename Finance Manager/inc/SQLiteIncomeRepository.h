#pragma once

#include "IIncomeRepository.h"
#include "IDatabaseConnection.h"

class SQLiteIncomeRepository : public IIncomeRepository {
public:
    explicit SQLiteIncomeRepository(IDatabaseConnection& db);
    void add(const Income& income) override;
    std::vector<Income> findAll(int userId) override;
private:
    IDatabaseConnection& db;
};
