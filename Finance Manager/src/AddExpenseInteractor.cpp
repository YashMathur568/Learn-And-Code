#include "AddExpenseInteractor.h"
#include <stdexcept>

AddExpenseInteractor::AddExpenseInteractor(IExpenseRepository& expenseRepository)
    : expenseRepository(expenseRepository) {}

void AddExpenseInteractor::execute(const Expense& expense) {
    if (expense.amount <= 0)
        throw std::runtime_error("Expense amount must be greater than zero");
    if (expense.description.empty())
        throw std::runtime_error("Description cannot be empty");
    if (expense.date.empty())
        throw std::runtime_error("Date cannot be empty");
    expenseRepository.add(expense);
}
