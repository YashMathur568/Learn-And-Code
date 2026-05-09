#pragma once

#include <vector>
#include "Expense.h"

class IGetExpensesInteractor {
public:
    virtual ~IGetExpensesInteractor() = default;
    virtual std::vector<Expense> execute(int userId) = 0;
};
