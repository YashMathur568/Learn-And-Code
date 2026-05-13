#include "FilterExpensesByCategoryInteractor.h"

FilterExpensesByCategoryInteractor::FilterExpensesByCategoryInteractor(IExpenseRepository& expenseRepository)
    : expenseRepository(expenseRepository) {}

std::vector<Expense> FilterExpensesByCategoryInteractor::execute(int userId, Category category) {
    return {};
}
