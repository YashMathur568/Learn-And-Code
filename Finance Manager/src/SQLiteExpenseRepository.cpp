#include "SQLiteExpenseRepository.h"
#include "CategoryUtils.h"
#include <stdexcept>

SQLiteExpenseRepository::SQLiteExpenseRepository(IDatabaseConnection& databaseConnection) : databaseConnection(databaseConnection) {}

void SQLiteExpenseRepository::add(const Expense& expense) {
    std::string sqlStatement =
        "INSERT INTO expenses (user_id, amount, category, description, date) VALUES ("
        + std::to_string(expense.userId) + ", "
        + std::to_string(expense.amount) + ", '"
        + categoryToString(expense.category) + "', '"
        + expense.description + "', '"
        + expense.date + "');";
    databaseConnection.execute(sqlStatement);
}

std::vector<Expense> SQLiteExpenseRepository::findAll(int userId) {
    std::string sqlStatement =
        "SELECT id, user_id, amount, category, description, date FROM expenses WHERE user_id = "
        + std::to_string(userId) + ";";
    ResultSet rows = databaseConnection.query(sqlStatement);
    std::vector<Expense> expenses;
    for (const auto& row : rows) {
        Expense expense;
        expense.id          = std::stoi(row[0]);
        expense.userId      = std::stoi(row[1]);
        expense.amount      = std::stod(row[2]);
        expense.category    = stringToCategory(row[3]);
        expense.description = row[4];
        expense.date        = row[5];
        expenses.push_back(expense);
    }
    return expenses;
}

std::vector<Expense> SQLiteExpenseRepository::findByCategory(int userId, Category category) {
    std::string sqlStatement =
        "SELECT id, user_id, amount, category, description, date FROM expenses WHERE user_id = "
        + std::to_string(userId) + " AND category = '" + categoryToString(category) + "';";
    ResultSet rows = databaseConnection.query(sqlStatement);
    std::vector<Expense> expenses;
    for (const auto& row : rows) {
        Expense expense;
        expense.id          = std::stoi(row[0]);
        expense.userId      = std::stoi(row[1]);
        expense.amount      = std::stod(row[2]);
        expense.category    = stringToCategory(row[3]);
        expense.description = row[4];
        expense.date        = row[5];
        expenses.push_back(expense);
    }
    return expenses;
}

std::vector<Expense> SQLiteExpenseRepository::findByDateRange(int userId, const std::string& from, const std::string& to) {
    std::string sqlStatement =
        "SELECT id, user_id, amount, category, description, date FROM expenses WHERE user_id = "
        + std::to_string(userId) + " AND date >= '" + from + "' AND date <= '" + to + "';";
    ResultSet rows = databaseConnection.query(sqlStatement);
    std::vector<Expense> expenses;
    for (const auto& row : rows) {
        Expense expense;
        expense.id          = std::stoi(row[0]);
        expense.userId      = std::stoi(row[1]);
        expense.amount      = std::stod(row[2]);
        expense.category    = stringToCategory(row[3]);
        expense.description = row[4];
        expense.date        = row[5];
        expenses.push_back(expense);
    }
    return expenses;
}

void SQLiteExpenseRepository::deleteById(int id) {
    std::string sqlStatement = "DELETE FROM expenses WHERE id = " + std::to_string(id) + ";";
    databaseConnection.execute(sqlStatement);
}
