#include "TrackBudgetInteractor.h"

TrackBudgetInteractor::TrackBudgetInteractor(IBudgetRepository& budgetRepo, IExpenseRepository& expenseRepo)
    : budgetRepo(budgetRepo), expenseRepo(expenseRepo) {}

BudgetStatus TrackBudgetInteractor::execute(int userId, Category category) {
    
    return {};
}
