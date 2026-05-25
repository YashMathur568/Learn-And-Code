#include "SetBudgetInteractor.h"
#include <stdexcept>

SetBudgetInteractor::SetBudgetInteractor(IBudgetRepository& budgetRepository)
    : budgetRepository(budgetRepository) {}

void SetBudgetInteractor::execute(const Budget& budget) {
    if (budget.limitAmount <= 0)
        throw std::runtime_error("Budget limit must be greater than zero");
    if (budget.month.empty())
        throw std::runtime_error("Month cannot be empty");
    budgetRepository.save(budget);
}
