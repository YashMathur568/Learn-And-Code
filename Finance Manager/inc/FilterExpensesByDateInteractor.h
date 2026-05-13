#pragma once

#include "IFilterExpensesByDateInteractor.h"
#include "IExpenseRepository.h"

class FilterExpensesByDateInteractor : public IFilterExpensesByDateInteractor {
public:
    explicit FilterExpensesByDateInteractor(IExpenseRepository& expenseRepository);
    std::vector<Expense> execute(int userId, const std::string& from, const std::string& to) override;
private:
    IExpenseRepository& expenseRepository;
};
