#include "GetIncomesInteractor.h"

GetIncomesInteractor::GetIncomesInteractor(IIncomeRepository& incomeRepository)
    : incomeRepository(incomeRepository) {}

std::vector<Income> GetIncomesInteractor::execute(int userId) {
    return {};
}
