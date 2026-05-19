#include "BudgetController.h"
#include "CategoryUtils.h"
#include <stdexcept>

static Category promptCategory(IInputHandler& inputHandler, IOutputHandler& outputHandler) {
    outputHandler.print("Categories: 1=FOOD  2=TRANSPORT  3=UTILITIES  4=ENTERTAINMENT  5=HEALTH  6=EDUCATION  7=OTHER");
    int choice = inputHandler.readInt("Select category: ");
    switch (choice) {
        case 1: return Category::FOOD;
        case 2: return Category::TRANSPORT;
        case 3: return Category::UTILITIES;
        case 4: return Category::ENTERTAINMENT;
        case 5: return Category::HEALTH;
        case 6: return Category::EDUCATION;
        default: return Category::OTHER;
    }
}

BudgetController::BudgetController(
    ISetBudgetInteractor& setInteractor,
    ITrackBudgetInteractor& trackInteractor,
    IInputHandler& inputHandler,
    IOutputHandler& outputHandler)
    : setInteractor(setInteractor), trackInteractor(trackInteractor), inputHandler(inputHandler), outputHandler(outputHandler) {}

void BudgetController::handleSetBudget(int userId) {
    outputHandler.print("\n--- Set Budget ---");
    Budget budget;
    budget.userId      = userId;
    budget.category    = promptCategory(inputHandler, outputHandler);
    budget.limitAmount = inputHandler.readDouble("Monthly limit amount: ");
    budget.month       = inputHandler.readString("Month (YYYY-MM): ");
    try {
        setInteractor.execute(budget);
        outputHandler.print("Budget saved.");
    } catch (const std::exception& exception) {
        outputHandler.printError(exception.what());
    }
}

void BudgetController::handleTrackBudget(int userId) {
    outputHandler.print("\n--- Track Budget ---");
    Category category = promptCategory(inputHandler, outputHandler);
    try {
        BudgetStatus status = trackInteractor.execute(userId, category);
        outputHandler.printTable(
            {"Category", "Limit", "Spent", "Remaining", "Exceeded"},
            {{
                categoryToString(status.category),
                std::to_string(status.limitAmount),
                std::to_string(status.spentAmount),
                std::to_string(status.remainingAmount),
                status.isExceeded ? "YES" : "NO"
            }}
        );
    } catch (const std::exception& exception) {
        outputHandler.printError(exception.what());
    }
}
