#include "GetSummaryInteractor.h"

GetSummaryInteractor::GetSummaryInteractor(IIncomeRepository& incomeRepository, IExpenseRepository& expenseRepository)
    : incomeRepository(incomeRepository), expenseRepository(expenseRepository) {}

FinancialSummary GetSummaryInteractor::execute(int userId) {
    return {};
}
