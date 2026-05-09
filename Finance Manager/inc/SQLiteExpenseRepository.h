#pragma once

#include "IExpenseRepository.h"
#include "IDatabaseConnection.h"

class SQLiteExpenseRepository : public IExpenseRepository {
public:
    explicit SQLiteExpenseRepository(IDatabaseConnection& db);
    void add(const Expense& expense) override;
    std::vector<Expense> findAll(int userId) override;
    std::vector<Expense> findByCategory(int userId, Category category) override;
    std::vector<Expense> findByDateRange(int userId, const std::string& from, const std::string& to) override;
    void deleteById(int id) override;
private:
    IDatabaseConnection& db;
};
