#pragma once

#include <vector>
#include "Expense.h"
#include "Category.h"

class IFilterExpensesByCategoryInteractor {
public:
    virtual ~IFilterExpensesByCategoryInteractor() = default;
    virtual std::vector<Expense> execute(int userId, Category category) = 0;
};
