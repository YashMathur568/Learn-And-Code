#pragma once

#include "IGetSummaryInteractor.h"
#include "IIncomeRepository.h"
#include "IExpenseRepository.h"

class GetSummaryInteractor : public IGetSummaryInteractor {
public:
    GetSummaryInteractor(IIncomeRepository& incomeRepo, IExpenseRepository& expenseRepo);
    FinancialSummary execute(int userId) override;
private:
    IIncomeRepository& incomeRepo;
    IExpenseRepository& expenseRepo;
};
