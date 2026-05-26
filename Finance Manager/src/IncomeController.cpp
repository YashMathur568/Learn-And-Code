#include "IncomeController.h"
#include "CategoryUtils.h"
#include <stdexcept>

IncomeController::IncomeController(
    IAddIncomeInteractor& addInteractor,
    IGetIncomesInteractor& getInteractor,
    IInputHandler& inputHandler,
    IOutputHandler& outputHandler)
    : addInteractor(addInteractor), getInteractor(getInteractor), inputHandler(inputHandler), outputHandler(outputHandler) {}

void IncomeController::handleAddIncome(int userId) {
    outputHandler.print("\n--- Add Income ---");
    Income income;
    income.userId      = userId;
    income.amount      = inputHandler.readDouble("Amount: ");
    income.source      = inputHandler.readString("Source: ");
    income.description = inputHandler.readString("Description: ");
    income.date        = inputHandler.readPastDate("Date (YYYY-MM-DD): ");
    try {
        addInteractor.execute(income);
        outputHandler.print("Income added.");
    } catch (const std::exception& exception) {
        outputHandler.printError(exception.what());
    }
}

void IncomeController::handleViewIncomes(int userId) {
    outputHandler.print("\n--- All Income ---");
    try {
        std::vector<Income> incomes = getInteractor.execute(userId);
        if (incomes.empty()) { outputHandler.print("No income records found."); return; }
        std::vector<std::vector<std::string>> rows;
        for (const Income& income : incomes)
            rows.push_back({
                std::to_string(income.incomeId),
                formatAmount(income.amount),
                income.source,
                income.description,
                income.date
            });
        outputHandler.printTable({"ID", "Amount", "Source", "Description", "Date"}, rows);
    } catch (const std::exception& exception) {
        outputHandler.printError(exception.what());
    }
}
