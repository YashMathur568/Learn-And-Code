#include "ExpenseController.h"

ExpenseController::ExpenseController(
    IAddExpenseInteractor& addInteractor,
    IGetExpensesInteractor& getInteractor,
    IFilterExpensesByCategoryInteractor& filterByCategoryInteractor,
    IFilterExpensesByDateInteractor& filterByDateInteractor,
    IDeleteExpenseInteractor& deleteInteractor,
    IInputHandler& inputHandler,
    IOutputHandler& outputHandler)
    : addInteractor(addInteractor), getInteractor(getInteractor),
      filterByCategoryInteractor(filterByCategoryInteractor), filterByDateInteractor(filterByDateInteractor),
      deleteInteractor(deleteInteractor), inputHandler(inputHandler), outputHandler(outputHandler) {}

void ExpenseController::handleAddExpense(int userId)        {}
void ExpenseController::handleViewExpenses(int userId)      {}
void ExpenseController::handleFilterByCategory(int userId)  {}
void ExpenseController::handleFilterByDate(int userId)      {}
void ExpenseController::handleDeleteExpense(int userId)     {}
