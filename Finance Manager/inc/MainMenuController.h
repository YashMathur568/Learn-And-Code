#pragma once

#include <string>
#include "ExpenseController.h"
#include "IncomeController.h"
#include "BudgetController.h"
#include "SummaryController.h"
#include "IValidateTokenInteractor.h"
#include "IInputHandler.h"
#include "IOutputHandler.h"

class MainMenuController {
public:
    MainMenuController(
        ExpenseController& expenseController,
        IncomeController& incomeController,
        BudgetController& budgetController,
        SummaryController& summaryController,
        IValidateTokenInteractor& validateToken,
        IInputHandler& input,
        IOutputHandler& output
    );
    void run(const std::string& token);
private:
    ExpenseController& expenseController;
    IncomeController& incomeController;
    BudgetController& budgetController;
    SummaryController& summaryController;
    IValidateTokenInteractor& validateToken;
    IInputHandler& input;
    IOutputHandler& output;
    std::string sessionToken;
    int currentUserId;

    void showMenu();
    void routeSelection(int choice);
};
