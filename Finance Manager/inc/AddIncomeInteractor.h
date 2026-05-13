#pragma once

#include "IAddIncomeInteractor.h"
#include "IIncomeRepository.h"

class AddIncomeInteractor : public IAddIncomeInteractor {
public:
    explicit AddIncomeInteractor(IIncomeRepository& incomeRepository);
    void execute(const Income& income) override;
private:
    IIncomeRepository& incomeRepository;
};
