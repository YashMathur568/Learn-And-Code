#include "AddIncomeInteractor.h"
#include <stdexcept>

AddIncomeInteractor::AddIncomeInteractor(IIncomeRepository& incomeRepository)
    : incomeRepository(incomeRepository) {}

void AddIncomeInteractor::execute(const Income& income) {
    if (income.amount <= 0)
        throw std::runtime_error("Income amount must be greater than zero");
    if (income.source.empty())
        throw std::runtime_error("Source cannot be empty");
    if (income.date.empty())
        throw std::runtime_error("Date cannot be empty");
    incomeRepository.add(income);
}
