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
      userRepository(databaseConnection), sessionRepository(databaseConnection) {}

AuthController AppFactory::createAuthController() {
    return AuthController(
        *new RegisterInteractor(userRepository),
        *new LoginInteractor(userRepository, sessionRepository),
        *new LogoutInteractor(sessionRepository),
        inputHandler, outputHandler
    );
}

MainMenuController AppFactory::createMainMenuController() {
    return MainMenuController(
        *new ExpenseController(
            *new AddExpenseInteractor(expenseRepository),
            *new GetExpensesInteractor(expenseRepository),
            *new FilterExpensesByCategoryInteractor(expenseRepository),
            *new FilterExpensesByDateInteractor(expenseRepository),
            *new DeleteExpenseInteractor(expenseRepository),
            inputHandler, outputHandler),
        *new IncomeController(*new AddIncomeInteractor(incomeRepository), *new GetIncomesInteractor(incomeRepository), inputHandler, outputHandler),
        *new BudgetController(*new SetBudgetInteractor(budgetRepository), *new TrackBudgetInteractor(budgetRepository, expenseRepository), inputHandler, outputHandler),
        *new SummaryController(*new GetSummaryInteractor(incomeRepository, expenseRepository), outputHandler),
        *new ValidateTokenInteractor(sessionRepository),
        inputHandler, outputHandler
    );
}
