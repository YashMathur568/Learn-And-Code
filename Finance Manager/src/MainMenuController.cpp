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
    sessionToken  = token;
    currentUserId = validateTokenInteractor.execute(token);
    if (currentUserId == -1) {
        outputHandler.printError("Invalid or expired session.");
        return;
    }
    bool running = true;
    while (running) {
        showMenu();
        int choice = inputHandler.readInt("Select option: ");
        if (choice == 5)
            running = false;
        else
            routeSelection(choice);
    }
}

void MainMenuController::showMenu() {
    outputHandler.print("\n======= Main Menu =======");
    outputHandler.print("1. Expenses");
    outputHandler.print("2. Income");
    outputHandler.print("3. Budget");
    outputHandler.print("4. Summary");
    outputHandler.print("5. Logout");
    outputHandler.print("=========================");
}

void MainMenuController::routeSelection(int choice) {
    switch (choice) {
        case 1: {
            bool inExpenseMenu = true;
            while (inExpenseMenu) {
                outputHandler.print("\n-- Expense Menu --");
                outputHandler.print("1. Add Expense");
                outputHandler.print("2. View All");
                outputHandler.print("3. Filter by Category");
                outputHandler.print("4. Filter by Date");
                outputHandler.print("5. Delete Expense");
                outputHandler.print("6. Back");
                int expenseChoice = inputHandler.readInt("Select option: ");
                switch (expenseChoice) {
                    case 1: expenseController.handleAddExpense(currentUserId);       break;
                    case 2: expenseController.handleViewExpenses(currentUserId);     break;
                    case 3: expenseController.handleFilterByCategory(currentUserId); break;
                    case 4: expenseController.handleFilterByDate(currentUserId);     break;
                    case 5: expenseController.handleDeleteExpense(currentUserId);    break;
                    default: inExpenseMenu = false; break;
                }
            }
            break;
        }
        case 2: {
            bool inIncomeMenu = true;
            while (inIncomeMenu) {
                outputHandler.print("\n-- Income Menu --");
                outputHandler.print("1. Add Income");
                outputHandler.print("2. View All");
                outputHandler.print("3. Back");
                int incomeChoice = inputHandler.readInt("Select option: ");
                switch (incomeChoice) {
                    case 1: incomeController.handleAddIncome(currentUserId);   break;
                    case 2: incomeController.handleViewIncomes(currentUserId); break;
                    default: inIncomeMenu = false; break;
                }
            }
            break;
        }
        case 3: {
            bool inBudgetMenu = true;
            while (inBudgetMenu) {
                outputHandler.print("\n-- Budget Menu --");
                outputHandler.print("1. Set Budget");
                outputHandler.print("2. Track Budget");
                outputHandler.print("3. Back");
                int budgetChoice = inputHandler.readInt("Select option: ");
                switch (budgetChoice) {
                    case 1: budgetController.handleSetBudget(currentUserId);   break;
                    case 2: budgetController.handleTrackBudget(currentUserId); break;
                    default: inBudgetMenu = false; break;
                }
            }
            break;
        }
        case 4:
            summaryController.handleShowSummary(currentUserId);
            break;
        default:
            outputHandler.printError("Invalid option.");
            break;
    }
}
