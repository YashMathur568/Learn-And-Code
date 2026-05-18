#include "GetExpensesInteractor.h"

GetExpensesInteractor::GetExpensesInteractor(IExpenseRepository& expenseRepository)
    : expenseRepository(expenseRepository) {}

std::vector<Expense> GetExpensesInteractor::execute(int userId) {
    return expenseRepository.findAll(userId);
}
