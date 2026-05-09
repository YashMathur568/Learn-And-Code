#pragma once

#include <vector>
#include <string>
#include "Expense.h"

class IFilterExpensesByDateInteractor {
public:
    virtual ~IFilterExpensesByDateInteractor() = default;
    virtual std::vector<Expense> execute(int userId, const std::string& from, const std::string& to) = 0;
};
