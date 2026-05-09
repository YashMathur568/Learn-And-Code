#include "GetExpensesInteractor.h"

GetExpensesInteractor::GetExpensesInteractor(IExpenseRepository& expenseRepo)
    : expenseRepo(expenseRepo) {}

std::vector<Expense> GetExpensesInteractor::execute(int userId) {
    
    return {};
}
