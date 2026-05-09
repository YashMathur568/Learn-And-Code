#include "FilterExpensesByCategoryInteractor.h"

FilterExpensesByCategoryInteractor::FilterExpensesByCategoryInteractor(IExpenseRepository& expenseRepo)
    : expenseRepo(expenseRepo) {}

std::vector<Expense> FilterExpensesByCategoryInteractor::execute(int userId, Category category) {
    
    return {};
}
