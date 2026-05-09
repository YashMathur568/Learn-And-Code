#include "AppFactory.h"

AppFactory::AppFactory(const std::string& dbPath)
    : db(dbPath),
      expenseRepo(db), incomeRepo(db), budgetRepo(db),
      userRepo(db), sessionRepo(db) {}

AuthController AppFactory::createAuthController() {
    
    return AuthController(
        *new RegisterInteractor(userRepo),
        *new LoginInteractor(userRepo, sessionRepo),
        *new LogoutInteractor(sessionRepo),
        input, output
    );
}

MainMenuController AppFactory::createMainMenuController() {
    
    return MainMenuController(
        *new ExpenseController(
            *new AddExpenseInteractor(expenseRepo),
            *new GetExpensesInteractor(expenseRepo),
            *new FilterExpensesByCategoryInteractor(expenseRepo),
            *new FilterExpensesByDateInteractor(expenseRepo),
            *new DeleteExpenseInteractor(expenseRepo),
            input, output),
        *new IncomeController(*new AddIncomeInteractor(incomeRepo), *new GetIncomesInteractor(incomeRepo), input, output),
        *new BudgetController(*new SetBudgetInteractor(budgetRepo), *new TrackBudgetInteractor(budgetRepo, expenseRepo), input, output),
        *new SummaryController(*new GetSummaryInteractor(incomeRepo, expenseRepo), output),
        *new ValidateTokenInteractor(sessionRepo),
        input, output
    );
}
