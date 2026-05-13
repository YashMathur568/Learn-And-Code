#pragma once

#include "ITrackBudgetInteractor.h"
#include "IBudgetRepository.h"
#include "IExpenseRepository.h"

class TrackBudgetInteractor : public ITrackBudgetInteractor {
public:
    TrackBudgetInteractor(IBudgetRepository& budgetRepository, IExpenseRepository& expenseRepository);
    BudgetStatus execute(int userId, Category category) override;
private:
    IBudgetRepository& budgetRepository;
    IExpenseRepository& expenseRepository;
};
