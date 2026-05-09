#pragma once

#include "BudgetStatus.h"
#include "Category.h"

class ITrackBudgetInteractor {
public:
    virtual ~ITrackBudgetInteractor() = default;
    virtual BudgetStatus execute(int userId, Category category) = 0;
};
