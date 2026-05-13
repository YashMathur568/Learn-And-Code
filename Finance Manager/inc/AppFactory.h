#pragma once

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

class AppFactory {
public:
    explicit AppFactory(const std::string& databasePath);
    AuthController      createAuthController();
    MainMenuController  createMainMenuController();
private:
    SQLiteConnection            databaseConnection;
    ConsoleInputHandler         inputHandler;
    ConsoleOutputHandler        outputHandler;
    SQLiteExpenseRepository     expenseRepository;
    SQLiteIncomeRepository      incomeRepository;
    SQLiteBudgetRepository      budgetRepository;
    SQLiteUserRepository        userRepository;
    SQLiteSessionRepository     sessionRepository;
};
