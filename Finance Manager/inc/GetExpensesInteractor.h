#pragma once

#include "IGetExpensesInteractor.h"
#include "IExpenseRepository.h"

class GetExpensesInteractor : public IGetExpensesInteractor {
public:
    explicit GetExpensesInteractor(IExpenseRepository& expenseRepo);
    std::vector<Expense> execute(int userId) override;
private:
    IExpenseRepository& expenseRepo;
};
