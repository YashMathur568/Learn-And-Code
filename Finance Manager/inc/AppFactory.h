#pragma once

#include <memory>
#include "SQLiteConnection.h"
#include "SQLiteExpenseRepository.h"
#include "SQLiteIncomeRepository.h"
#include "SQLiteBudgetRepository.h"
#include "SQLiteUserRepository.h"
#include "SQLiteSessionRepository.h"
#include "ConsoleInputHandler.h"
#include "ConsoleOutputHandler.h"
#include "AuthController.h"
#include "ExpenseController.h"
#include "IncomeController.h"
#include "BudgetController.h"
#include "SummaryController.h"
#include "MainMenuController.h"
#include "IRegisterInteractor.h"
#include "ILoginInteractor.h"
#include "ILogoutInteractor.h"
#include "IValidateTokenInteractor.h"
#include "IAddExpenseInteractor.h"
#include "IGetExpensesInteractor.h"
#include "IFilterExpensesByCategoryInteractor.h"
#include "IFilterExpensesByDateInteractor.h"
#include "IDeleteExpenseInteractor.h"
#include "IAddIncomeInteractor.h"
#include "IGetIncomesInteractor.h"
#include "ISetBudgetInteractor.h"
#include "ITrackBudgetInteractor.h"
#include "IGetSummaryInteractor.h"

class AppFactory {
public:
    explicit AppFactory(const std::string& databasePath);
    AuthController&      getAuthController();
    MainMenuController&  getMainMenuController();
private:
    SQLiteConnection            databaseConnection;
    SQLiteExpenseRepository     expenseRepository;
    SQLiteIncomeRepository      incomeRepository;
    SQLiteBudgetRepository      budgetRepository;
    SQLiteUserRepository        userRepository;
    SQLiteSessionRepository     sessionRepository;
    ConsoleInputHandler         inputHandler;
    ConsoleOutputHandler        outputHandler;

    std::unique_ptr<IRegisterInteractor>                  registerInteractor;
    std::unique_ptr<ILoginInteractor>                     loginInteractor;
    std::unique_ptr<ILogoutInteractor>                    logoutInteractor;
    std::unique_ptr<IValidateTokenInteractor>             validateTokenInteractor;
    std::unique_ptr<IAddExpenseInteractor>                addExpenseInteractor;
    std::unique_ptr<IGetExpensesInteractor>               getExpensesInteractor;
    std::unique_ptr<IFilterExpensesByCategoryInteractor>  filterByCategoryInteractor;
    std::unique_ptr<IFilterExpensesByDateInteractor>      filterByDateInteractor;
    std::unique_ptr<IDeleteExpenseInteractor>             deleteExpenseInteractor;
    std::unique_ptr<IAddIncomeInteractor>                 addIncomeInteractor;
    std::unique_ptr<IGetIncomesInteractor>                getIncomesInteractor;
    std::unique_ptr<ISetBudgetInteractor>                 setBudgetInteractor;
    std::unique_ptr<ITrackBudgetInteractor>               trackBudgetInteractor;
    std::unique_ptr<IGetSummaryInteractor>                getSummaryInteractor;

    std::unique_ptr<ExpenseController>  expenseController;
    std::unique_ptr<IncomeController>   incomeController;
    std::unique_ptr<BudgetController>   budgetController;
    std::unique_ptr<SummaryController>  summaryController;
    std::unique_ptr<AuthController>     authController;
    std::unique_ptr<MainMenuController> mainMenuController;
};
