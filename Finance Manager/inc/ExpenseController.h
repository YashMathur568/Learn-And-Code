#pragma once

#include "IAddExpenseInteractor.h"
#include "IGetExpensesInteractor.h"
#include "IFilterExpensesByCategoryInteractor.h"
#include "IFilterExpensesByDateInteractor.h"
#include "IDeleteExpenseInteractor.h"
#include "IInputHandler.h"
#include "IOutputHandler.h"

class ExpenseController {
public:
    ExpenseController(
        IAddExpenseInteractor& addInteractor,
        IGetExpensesInteractor& getInteractor,
        IFilterExpensesByCategoryInteractor& filterByCategoryInteractor,
        IFilterExpensesByDateInteractor& filterByDateInteractor,
        IDeleteExpenseInteractor& deleteInteractor,
        IInputHandler& inputHandler,
        IOutputHandler& outputHandler
    );
    void handleAddExpense(int userId);
    void handleViewExpenses(int userId);
    void handleFilterByCategory(int userId);
    void handleFilterByDate(int userId);
    void handleDeleteExpense(int userId);
private:
    IAddExpenseInteractor& addInteractor;
    IGetExpensesInteractor& getInteractor;
    IFilterExpensesByCategoryInteractor& filterByCategoryInteractor;
    IFilterExpensesByDateInteractor& filterByDateInteractor;
    IDeleteExpenseInteractor& deleteInteractor;
    IInputHandler& inputHandler;
    IOutputHandler& outputHandler;
};
