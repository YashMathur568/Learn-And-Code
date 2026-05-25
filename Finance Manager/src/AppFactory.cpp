#include "AppFactory.h"
#include "RegisterInteractor.h"
#include "LoginInteractor.h"
#include "LogoutInteractor.h"
#include "ValidateTokenInteractor.h"
#include "AddExpenseInteractor.h"
#include "GetExpensesInteractor.h"
#include "FilterExpensesByCategoryInteractor.h"
#include "FilterExpensesByDateInteractor.h"
#include "DeleteExpenseInteractor.h"
#include "AddIncomeInteractor.h"
#include "GetIncomesInteractor.h"
#include "SetBudgetInteractor.h"
#include "TrackBudgetInteractor.h"
#include "GetSummaryInteractor.h"

AppFactory::AppFactory(const std::string& databasePath)
    : databaseConnection(databasePath),
      expenseRepository(databaseConnection), incomeRepository(databaseConnection), budgetRepository(databaseConnection),
      userRepository(databaseConnection), sessionRepository(databaseConnection) {

    registerInteractor        = std::make_unique<RegisterInteractor>(userRepository);
    loginInteractor           = std::make_unique<LoginInteractor>(userRepository, sessionRepository);
    logoutInteractor          = std::make_unique<LogoutInteractor>(sessionRepository);
    validateTokenInteractor   = std::make_unique<ValidateTokenInteractor>(sessionRepository);

    addExpenseInteractor      = std::make_unique<AddExpenseInteractor>(expenseRepository);
    getExpensesInteractor     = std::make_unique<GetExpensesInteractor>(expenseRepository);
    filterByCategoryInteractor = std::make_unique<FilterExpensesByCategoryInteractor>(expenseRepository);
    filterByDateInteractor    = std::make_unique<FilterExpensesByDateInteractor>(expenseRepository);
    deleteExpenseInteractor   = std::make_unique<DeleteExpenseInteractor>(expenseRepository);
    addIncomeInteractor       = std::make_unique<AddIncomeInteractor>(incomeRepository);
    getIncomesInteractor      = std::make_unique<GetIncomesInteractor>(incomeRepository);
    setBudgetInteractor       = std::make_unique<SetBudgetInteractor>(budgetRepository);
    trackBudgetInteractor     = std::make_unique<TrackBudgetInteractor>(budgetRepository, expenseRepository);
    getSummaryInteractor      = std::make_unique<GetSummaryInteractor>(incomeRepository, expenseRepository);

    expenseController = std::make_unique<ExpenseController>(
        *addExpenseInteractor, *getExpensesInteractor,
        *filterByCategoryInteractor, *filterByDateInteractor,
        *deleteExpenseInteractor, inputHandler, outputHandler);
    incomeController  = std::make_unique<IncomeController>(
        *addIncomeInteractor, *getIncomesInteractor, inputHandler, outputHandler);
    budgetController  = std::make_unique<BudgetController>(
        *setBudgetInteractor, *trackBudgetInteractor, inputHandler, outputHandler);
    summaryController = std::make_unique<SummaryController>(
        *getSummaryInteractor, outputHandler);

    authController = std::make_unique<AuthController>(
        *registerInteractor, *loginInteractor, *logoutInteractor, inputHandler, outputHandler);
    mainMenuController = std::make_unique<MainMenuController>(
        *expenseController, *incomeController, *budgetController, *summaryController,
        *validateTokenInteractor, inputHandler, outputHandler);
}

AuthController& AppFactory::getAuthController() {
    return *authController;
}

MainMenuController& AppFactory::getMainMenuController() {
    return *mainMenuController;
}
