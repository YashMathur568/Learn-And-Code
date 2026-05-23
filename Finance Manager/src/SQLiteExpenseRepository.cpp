#include "SQLiteExpenseRepository.h"
#include "CategoryUtils.h"
#include <stdexcept>

SQLiteExpenseRepository::SQLiteExpenseRepository(IDatabaseConnection& databaseConnection) : databaseConnection(databaseConnection) {}

void SQLiteExpenseRepository::add(const Expense& expense) {
    databaseConnection.executeParameterized(
        "INSERT INTO expenses (user_id, amount, category, description, date) VALUES (?, ?, ?, ?, ?);",
        { expense.userId, expense.amount, categoryToString(expense.category), expense.description, expense.date }
    );
}

std::vector<Expense> SQLiteExpenseRepository::findAll(int userId) {
    ResultSet rows = databaseConnection.queryParameterized(
        "SELECT id, user_id, amount, category, description, date FROM expenses WHERE user_id = ?;",
        { userId }
    );
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
    ResultSet rows = databaseConnection.queryParameterized(
        "SELECT id, user_id, amount, category, description, date FROM expenses WHERE user_id = ? AND category = ?;",
        { userId, categoryToString(category) }
    );
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
    ResultSet rows = databaseConnection.queryParameterized(
        "SELECT id, user_id, amount, category, description, date FROM expenses WHERE user_id = ? AND date >= ? AND date <= ?;",
        { userId, from, to }
    );
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
    databaseConnection.executeParameterized(
        "DELETE FROM expenses WHERE id = ?;",
        { id }
    );
}
