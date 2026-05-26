#pragma once

#include "IGetSummaryInteractor.h"
#include "IIncomeRepository.h"
#include "IExpenseRepository.h"

class GetSummaryInteractor : public IGetSummaryInteractor {
public:
    GetSummaryInteractor(IIncomeRepository& incomeRepository, IExpenseRepository& expenseRepository);
    FinancialSummary execute(int userId) override;
    FinancialSummary executeForMonth(int userId, const std::string& month) override;
private:
    IIncomeRepository& incomeRepository;
    IExpenseRepository& expenseRepository;
};
