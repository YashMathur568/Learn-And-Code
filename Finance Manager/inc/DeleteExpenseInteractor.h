#pragma once

#include "IDeleteExpenseInteractor.h"
#include "IExpenseRepository.h"

class DeleteExpenseInteractor : public IDeleteExpenseInteractor {
public:
    explicit DeleteExpenseInteractor(IExpenseRepository& expenseRepository);
    void execute(int id) override;
private:
    IExpenseRepository& expenseRepository;
};
