#pragma once

#include "Expense.h"

class IAddExpenseInteractor {
public:
    virtual ~IAddExpenseInteractor() = default;
    virtual void execute(const Expense& expense) = 0;
};
