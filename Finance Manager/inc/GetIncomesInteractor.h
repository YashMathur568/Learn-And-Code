#pragma once

#include "IGetIncomesInteractor.h"
#include "IIncomeRepository.h"

class GetIncomesInteractor : public IGetIncomesInteractor {
public:
    explicit GetIncomesInteractor(IIncomeRepository& incomeRepo);
    std::vector<Income> execute(int userId) override;
private:
    IIncomeRepository& incomeRepo;
};
