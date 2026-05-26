#pragma once

#include "IAddExpenseInteractor.h"
#include "IExpenseRepository.h"

class AddExpenseInteractor : public IAddExpenseInteractor {
public:
    explicit AddExpenseInteractor(IExpenseRepository& expenseRepository);
    void execute(const Expense& expense) override;
private:
    IExpenseRepository& expenseRepository;
};
