#pragma once

#include "IAddExpenseInteractor.h"
#include "IExpenseRepository.h"

class AddExpenseInteractor : public IAddExpenseInteractor {
public:
    explicit AddExpenseInteractor(IExpenseRepository& expenseRepo);
    void execute(const Expense& expense) override;
private:
    IExpenseRepository& expenseRepo;
};
