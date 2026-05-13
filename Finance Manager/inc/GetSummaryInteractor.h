#pragma once

#include "IGetSummaryInteractor.h"
#include "IIncomeRepository.h"
#include "IExpenseRepository.h"

class GetSummaryInteractor : public IGetSummaryInteractor {
public:
    GetSummaryInteractor(IIncomeRepository& incomeRepository, IExpenseRepository& expenseRepository);
    FinancialSummary execute(int userId) override;
private:
    IIncomeRepository& incomeRepository;
    IExpenseRepository& expenseRepository;
};
