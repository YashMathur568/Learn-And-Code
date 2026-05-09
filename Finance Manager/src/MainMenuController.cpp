#include "MainMenuController.h"

MainMenuController::MainMenuController(
    ExpenseController& expenseController,
    IncomeController& incomeController,
    BudgetController& budgetController,
    SummaryController& summaryController,
    IValidateTokenInteractor& validateToken,
    IInputHandler& input,
    IOutputHandler& output)
    : expenseController(expenseController), incomeController(incomeController),
      budgetController(budgetController), summaryController(summaryController),
      validateToken(validateToken), input(input), output(output),
      currentUserId(-1) {}

void MainMenuController::run(const std::string& token) {
    
}

void MainMenuController::showMenu()                {  }
void MainMenuController::routeSelection(int choice){  }
