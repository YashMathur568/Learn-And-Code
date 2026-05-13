#include "SQLiteIncomeRepository.h"

SQLiteIncomeRepository::SQLiteIncomeRepository(IDatabaseConnection& databaseConnection) : databaseConnection(databaseConnection) {}

void SQLiteIncomeRepository::add(const Income& income) {
    std::string sqlStatement =
        "INSERT INTO income (user_id, amount, source, description, date) VALUES ("
        + std::to_string(income.userId) + ", "
        + std::to_string(income.amount) + ", '"
        + income.source + "', '"
        + income.description + "', '"
        + income.date + "');";
    databaseConnection.execute(sqlStatement);
}

std::vector<Income> SQLiteIncomeRepository::findAll(int userId) {
    std::string sqlStatement =
        "SELECT id, user_id, amount, source, description, date FROM income WHERE user_id = "
        + std::to_string(userId) + ";";
    ResultSet rows = databaseConnection.query(sqlStatement);
    std::vector<Income> incomes;
    for (const auto& row : rows) {
        Income income;
        income.id          = std::stoi(row[0]);
        income.userId      = std::stoi(row[1]);
        income.amount      = std::stod(row[2]);
        income.source      = row[3];
        income.description = row[4];
        income.date        = row[5];
        incomes.push_back(income);
    }
    return incomes;
}
