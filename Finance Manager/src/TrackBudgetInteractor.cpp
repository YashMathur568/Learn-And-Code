#include "TrackBudgetInteractor.h"

TrackBudgetInteractor::TrackBudgetInteractor(IBudgetRepository& budgetRepository, IExpenseRepository& expenseRepository)
    : budgetRepository(budgetRepository), expenseRepository(expenseRepository) {}

BudgetStatus TrackBudgetInteractor::execute(int userId, Category category, const std::string& month) {
    Budget budget = budgetRepository.findByCategory(userId, category, month);
    std::vector<Expense> expenses = expenseRepository.findByCategory(userId, category);
    double totalSpent = 0.0;
    for (const Expense& expense : expenses)
        if (expense.date.size() >= 7 && expense.date.substr(0, 7) == month)
            totalSpent += expense.amount;
    BudgetStatus status;
    status.category        = category;
    status.limitAmount     = budget.limitAmount;
    status.spentAmount     = totalSpent;
    status.remainingAmount = budget.limitAmount - totalSpent;
    status.isExceeded      = totalSpent > budget.limitAmount;
    return status;
}
