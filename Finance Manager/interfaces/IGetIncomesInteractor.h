#pragma once

#include <vector>
#include "Income.h"

class IGetIncomesInteractor {
public:
    virtual ~IGetIncomesInteractor() = default;
    virtual std::vector<Income> execute(int userId) = 0;
};
