#pragma once

#include "ITrackBudgetInteractor.h"
#include "IBudgetRepository.h"
#include "IExpenseRepository.h"

class TrackBudgetInteractor : public ITrackBudgetInteractor {
public:
    TrackBudgetInteractor(IBudgetRepository& budgetRepo, IExpenseRepository& expenseRepo);
    BudgetStatus execute(int userId, Category category) override;
private:
    IBudgetRepository& budgetRepo;
    IExpenseRepository& expenseRepo;
};
