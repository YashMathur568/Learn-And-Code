#pragma once

#include "IDeleteExpenseInteractor.h"
#include "IExpenseRepository.h"

class DeleteExpenseInteractor : public IDeleteExpenseInteractor {
public:
    explicit DeleteExpenseInteractor(IExpenseRepository& expenseRepo);
    void execute(int id) override;
private:
    IExpenseRepository& expenseRepo;
};
