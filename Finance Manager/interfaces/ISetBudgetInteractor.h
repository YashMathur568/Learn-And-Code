#pragma once

#include "Budget.h"

class ISetBudgetInteractor {
public:
    virtual ~ISetBudgetInteractor() = default;
    virtual void execute(const Budget& budget) = 0;
};
