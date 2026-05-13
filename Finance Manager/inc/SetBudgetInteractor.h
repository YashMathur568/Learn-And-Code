#pragma once

#include "ISetBudgetInteractor.h"
#include "IBudgetRepository.h"

class SetBudgetInteractor : public ISetBudgetInteractor {
public:
    explicit SetBudgetInteractor(IBudgetRepository& budgetRepository);
    void execute(const Budget& budget) override;
private:
    IBudgetRepository& budgetRepository;
};
