#include "ExpenseController.h"

ExpenseController::ExpenseController(
    IAddExpenseInteractor& addInteractor,
    IGetExpensesInteractor& getInteractor,
    IFilterExpensesByCategoryInteractor& filterByCatInteractor,
    IFilterExpensesByDateInteractor& filterByDateInteractor,
    IDeleteExpenseInteractor& deleteInteractor,
    IInputHandler& input,
    IOutputHandler& output)
    : addInteractor(addInteractor), getInteractor(getInteractor),
      filterByCatInteractor(filterByCatInteractor), filterByDateInteractor(filterByDateInteractor),
      deleteInteractor(deleteInteractor), input(input), output(output) {}

void ExpenseController::handleAddExpense(int userId)        {  }
void ExpenseController::handleViewExpenses(int userId)      {  }
void ExpenseController::handleFilterByCategory(int userId)  {  }
void ExpenseController::handleFilterByDate(int userId)      {  }
void ExpenseController::handleDeleteExpense(int userId)     {  }
