#include "SQLiteBudgetRepository.h"
#include "CategoryUtils.h"
#include <stdexcept>

SQLiteBudgetRepository::SQLiteBudgetRepository(IDatabaseConnection& databaseConnection) : databaseConnection(databaseConnection) {}

void SQLiteBudgetRepository::save(const Budget& budget) {
    databaseConnection.executeParameterized(
        "INSERT OR REPLACE INTO budgets (user_id, category, limit_amount, month) VALUES (?, ?, ?, ?);",
        { budget.userId, categoryToString(budget.category), budget.limitAmount, budget.month }
    );
}

Budget SQLiteBudgetRepository::findByCategory(int userId, Category category) {
    ResultSet rows = databaseConnection.queryParameterized(
        "SELECT id, user_id, category, limit_amount, month FROM budgets WHERE user_id = ? AND category = ?;",
        { userId, categoryToString(category) }
    );
    if (rows.empty())
        throw std::runtime_error("Budget not found for given category");
    const auto& row = rows[0];
    Budget budget;
    budget.id          = std::stoi(row[0]);
    budget.userId      = std::stoi(row[1]);
    budget.category    = stringToCategory(row[2]);
    budget.limitAmount = std::stod(row[3]);
    budget.month       = row[4];
    return budget;
}

std::vector<Budget> SQLiteBudgetRepository::findAll(int userId) {
    ResultSet rows = databaseConnection.queryParameterized(
        "SELECT id, user_id, category, limit_amount, month FROM budgets WHERE user_id = ?;",
        { userId }
    );
    std::vector<Budget> budgets;
    for (const auto& row : rows) {
        Budget budget;
        budget.id          = std::stoi(row[0]);
        budget.userId      = std::stoi(row[1]);
        budget.category    = stringToCategory(row[2]);
        budget.limitAmount = std::stod(row[3]);
        budget.month       = row[4];
        budgets.push_back(budget);
    }
    return budgets;
}
