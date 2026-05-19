#include "ExpenseController.h"
#include "CategoryUtils.h"
#include <stdexcept>

static Category promptCategory(IInputHandler& inputHandler, IOutputHandler& outputHandler) {
    outputHandler.print("Categories: 1=FOOD  2=TRANSPORT  3=UTILITIES  4=ENTERTAINMENT  5=HEALTH  6=EDUCATION  7=OTHER");
    int choice = inputHandler.readInt("Select category: ");
    switch (choice) {
        case 1: return Category::FOOD;
        case 2: return Category::TRANSPORT;
        case 3: return Category::UTILITIES;
        case 4: return Category::ENTERTAINMENT;
        case 5: return Category::HEALTH;
        case 6: return Category::EDUCATION;
        default: return Category::OTHER;
    }
}

static std::vector<std::vector<std::string>> expensesToRows(const std::vector<Expense>& expenses) {
    std::vector<std::vector<std::string>> rows;
    for (const Expense& expense : expenses)
        rows.push_back({
            std::to_string(expense.id),
            std::to_string(expense.amount),
            categoryToString(expense.category),
            expense.description,
            expense.date
        });
    return rows;
}

ExpenseController::ExpenseController(
    IAddExpenseInteractor& addInteractor,
    IGetExpensesInteractor& getInteractor,
    IFilterExpensesByCategoryInteractor& filterByCategoryInteractor,
    IFilterExpensesByDateInteractor& filterByDateInteractor,
    IDeleteExpenseInteractor& deleteInteractor,
    IInputHandler& inputHandler,
    IOutputHandler& outputHandler)
    : addInteractor(addInteractor), getInteractor(getInteractor),
      filterByCategoryInteractor(filterByCategoryInteractor), filterByDateInteractor(filterByDateInteractor),
      deleteInteractor(deleteInteractor), inputHandler(inputHandler), outputHandler(outputHandler) {}

void ExpenseController::handleAddExpense(int userId) {
    outputHandler.print("\n--- Add Expense ---");
    Expense expense;
    expense.userId      = userId;
    expense.amount      = inputHandler.readDouble("Amount: ");
    expense.category    = promptCategory(inputHandler, outputHandler);
    expense.description = inputHandler.readString("Description: ");
    expense.date        = inputHandler.readDate("Date (YYYY-MM-DD): ");
    try {
        addInteractor.execute(expense);
        outputHandler.print("Expense added.");
    } catch (const std::exception& exception) {
        outputHandler.printError(exception.what());
    }
}

void ExpenseController::handleViewExpenses(int userId) {
    outputHandler.print("\n--- All Expenses ---");
    try {
        std::vector<Expense> expenses = getInteractor.execute(userId);
        if (expenses.empty()) { outputHandler.print("No expenses found."); return; }
        outputHandler.printTable({"ID", "Amount", "Category", "Description", "Date"}, expensesToRows(expenses));
    } catch (const std::exception& exception) {
        outputHandler.printError(exception.what());
    }
}

void ExpenseController::handleFilterByCategory(int userId) {
    outputHandler.print("\n--- Filter Expenses by Category ---");
    Category category = promptCategory(inputHandler, outputHandler);
    try {
        std::vector<Expense> expenses = filterByCategoryInteractor.execute(userId, category);
        if (expenses.empty()) { outputHandler.print("No expenses found."); return; }
        outputHandler.printTable({"ID", "Amount", "Category", "Description", "Date"}, expensesToRows(expenses));
    } catch (const std::exception& exception) {
        outputHandler.printError(exception.what());
    }
}

void ExpenseController::handleFilterByDate(int userId) {
    outputHandler.print("\n--- Filter Expenses by Date ---");
    std::string from = inputHandler.readDate("From (YYYY-MM-DD): ");
    std::string to   = inputHandler.readDate("To   (YYYY-MM-DD): ");
    try {
        std::vector<Expense> expenses = filterByDateInteractor.execute(userId, from, to);
        if (expenses.empty()) { outputHandler.print("No expenses found."); return; }
        outputHandler.printTable({"ID", "Amount", "Category", "Description", "Date"}, expensesToRows(expenses));
    } catch (const std::exception& exception) {
        outputHandler.printError(exception.what());
    }
}

void ExpenseController::handleDeleteExpense(int userId) {
    outputHandler.print("\n--- Delete Expense ---");
    handleViewExpenses(userId);
    int expenseId = inputHandler.readInt("Enter Expense ID to delete: ");
    try {
        deleteInteractor.execute(expenseId);
        outputHandler.print("Expense deleted.");
    } catch (const std::exception& exception) {
        outputHandler.printError(exception.what());
    }
}
