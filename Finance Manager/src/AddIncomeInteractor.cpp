#include "AddIncomeInteractor.h"

AddIncomeInteractor::AddIncomeInteractor(IIncomeRepository& incomeRepository)
    : incomeRepository(incomeRepository) {}

void AddIncomeInteractor::execute(const Income& income) {
    incomeRepository.add(income);
}
