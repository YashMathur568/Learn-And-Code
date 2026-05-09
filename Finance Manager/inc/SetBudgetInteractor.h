#pragma once

#include "ISetBudgetInteractor.h"
#include "IBudgetRepository.h"

class SetBudgetInteractor : public ISetBudgetInteractor {
public:
    explicit SetBudgetInteractor(IBudgetRepository& budgetRepo);
    void execute(const Budget& budget) override;
private:
    IBudgetRepository& budgetRepo;
};
