#pragma once

#include "IGetIncomesInteractor.h"
#include "IIncomeRepository.h"

class GetIncomesInteractor : public IGetIncomesInteractor {
public:
    explicit GetIncomesInteractor(IIncomeRepository& incomeRepository);
    std::vector<Income> execute(int userId) override;
private:
    IIncomeRepository& incomeRepository;
};
