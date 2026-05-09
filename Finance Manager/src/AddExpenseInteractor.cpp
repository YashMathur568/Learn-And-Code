#include "AddExpenseInteractor.h"

AddExpenseInteractor::AddExpenseInteractor(IExpenseRepository& expenseRepo)
    : expenseRepo(expenseRepo) {}

void AddExpenseInteractor::execute(const Expense& expense) {
    
}
