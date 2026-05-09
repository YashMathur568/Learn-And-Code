#include "GetSummaryInteractor.h"

GetSummaryInteractor::GetSummaryInteractor(IIncomeRepository& incomeRepo, IExpenseRepository& expenseRepo)
    : incomeRepo(incomeRepo), expenseRepo(expenseRepo) {}

FinancialSummary GetSummaryInteractor::execute(int userId) {
    
    return {};
}
