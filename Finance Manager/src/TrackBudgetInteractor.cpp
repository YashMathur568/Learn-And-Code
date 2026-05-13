#include "TrackBudgetInteractor.h"

TrackBudgetInteractor::TrackBudgetInteractor(IBudgetRepository& budgetRepository, IExpenseRepository& expenseRepository)
    : budgetRepository(budgetRepository), expenseRepository(expenseRepository) {}

BudgetStatus TrackBudgetInteractor::execute(int userId, Category category) {
    return {};
}
