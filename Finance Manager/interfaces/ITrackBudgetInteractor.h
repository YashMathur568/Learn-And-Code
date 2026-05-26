#pragma once

#include <string>
#include "BudgetStatus.h"
#include "Category.h"

class ITrackBudgetInteractor {
public:
    virtual ~ITrackBudgetInteractor() = default;
    virtual BudgetStatus execute(int userId, Category category, const std::string& month) = 0;
};
