#include "FilterExpensesByDateInteractor.h"

FilterExpensesByDateInteractor::FilterExpensesByDateInteractor(IExpenseRepository& expenseRepo)
    : expenseRepo(expenseRepo) {}

std::vector<Expense> FilterExpensesByDateInteractor::execute(int userId, const std::string& from, const std::string& to) {
    
    return {};
}
