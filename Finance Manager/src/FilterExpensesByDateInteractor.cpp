#include "FilterExpensesByDateInteractor.h"

FilterExpensesByDateInteractor::FilterExpensesByDateInteractor(IExpenseRepository& expenseRepository)
    : expenseRepository(expenseRepository) {}

std::vector<Expense> FilterExpensesByDateInteractor::execute(int userId, const std::string& from, const std::string& to) {
    return {};
}
