#include "MainMenuController.h"

MainMenuController::MainMenuController(
    ExpenseController& expenseController,
    IncomeController& incomeController,
    BudgetController& budgetController,
    SummaryController& summaryController,
    IValidateTokenInteractor& validateTokenInteractor,
    IInputHandler& inputHandler,
    IOutputHandler& outputHandler)
    : expenseController(expenseController), incomeController(incomeController),
      budgetController(budgetController), summaryController(summaryController),
      validateTokenInteractor(validateTokenInteractor), inputHandler(inputHandler), outputHandler(outputHandler),
      currentUserId(-1) {}

void MainMenuController::run(const std::string& token) {
}

void MainMenuController::showMenu()                {}
void MainMenuController::routeSelection(int choice){}
