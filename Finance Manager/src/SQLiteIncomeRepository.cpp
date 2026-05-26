#include "SQLiteIncomeRepository.h"

SQLiteIncomeRepository::SQLiteIncomeRepository(IDatabaseConnection& databaseConnection) : databaseConnection(databaseConnection) {}

void SQLiteIncomeRepository::add(const Income& income) {
    databaseConnection.executeParameterized(
        "INSERT INTO income (user_id, amount, source, description, date) VALUES (?, ?, ?, ?, ?);",
        { income.userId, income.amount, income.source, income.description, income.date }
    );
}

std::vector<Income> SQLiteIncomeRepository::findAll(int userId) {
    ResultSet rows = databaseConnection.queryParameterized(
        "SELECT id, user_id, amount, source, description, date FROM income WHERE user_id = ?;",
        { userId }
    );
    std::vector<Income> incomes;
    for (const auto& row : rows) {
        Income income;
        income.incomeId     = std::stoi(row[0]);
        income.userId      = std::stoi(row[1]);
        income.amount      = std::stod(row[2]);
        income.source      = row[3];
        income.description = row[4];
        income.date        = row[5];
        incomes.push_back(income);
    }
    return incomes;
}
