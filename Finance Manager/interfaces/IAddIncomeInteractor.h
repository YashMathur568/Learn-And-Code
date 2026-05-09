#pragma once

#include "Income.h"

class IAddIncomeInteractor {
public:
    virtual ~IAddIncomeInteractor() = default;
    virtual void execute(const Income& income) = 0;
};
