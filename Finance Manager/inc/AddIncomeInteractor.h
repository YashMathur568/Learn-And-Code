#pragma once

#include "IAddIncomeInteractor.h"
#include "IIncomeRepository.h"

class AddIncomeInteractor : public IAddIncomeInteractor {
public:
    explicit AddIncomeInteractor(IIncomeRepository& incomeRepo);
    void execute(const Income& income) override;
private:
    IIncomeRepository& incomeRepo;
};
