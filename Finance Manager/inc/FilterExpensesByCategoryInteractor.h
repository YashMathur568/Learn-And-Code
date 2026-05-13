#pragma once

#include "IFilterExpensesByCategoryInteractor.h"
#include "IExpenseRepository.h"

class FilterExpensesByCategoryInteractor : public IFilterExpensesByCategoryInteractor {
public:
    explicit FilterExpensesByCategoryInteractor(IExpenseRepository& expenseRepository);
    std::vector<Expense> execute(int userId, Category category) override;
private:
    IExpenseRepository& expenseRepository;
};
