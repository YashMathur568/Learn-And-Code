#include "AddExpenseInteractor.h"

AddExpenseInteractor::AddExpenseInteractor(IExpenseRepository& expenseRepository)
    : expenseRepository(expenseRepository) {}

void AddExpenseInteractor::execute(const Expense& expense) {
    expenseRepository.add(expense);
}
