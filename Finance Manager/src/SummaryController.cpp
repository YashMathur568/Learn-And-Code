#include "SummaryController.h"
#include <stdexcept>
#include <cctype>

SummaryController::SummaryController(IGetSummaryInteractor& summaryInteractor, IInputHandler& inputHandler, IOutputHandler& outputHandler)
    : summaryInteractor(summaryInteractor), inputHandler(inputHandler), outputHandler(outputHandler) {}

void SummaryController::handleShowSummary(int userId) {
    try {
        FinancialSummary summary = summaryInteractor.execute(userId);
        outputHandler.print("\n--- Complete Summary ---");
        outputHandler.printSummary(summary);
    } catch (const std::exception& exception) {
        outputHandler.printError(exception.what());
    }
}

void SummaryController::handleShowMonthlySummary(int userId) {
    std::string month;
    while (true) {
        month = inputHandler.readString("Enter month (YYYY-MM): ");
        if (month.size() == 7 && month[4] == '-'
            && std::isdigit(static_cast<unsigned char>(month[0]))
            && std::isdigit(static_cast<unsigned char>(month[1]))
            && std::isdigit(static_cast<unsigned char>(month[2]))
            && std::isdigit(static_cast<unsigned char>(month[3]))
            && std::isdigit(static_cast<unsigned char>(month[5]))
            && std::isdigit(static_cast<unsigned char>(month[6])))
            break;
        outputHandler.printError("Invalid format. Use YYYY-MM (e.g. 2026-05).");
    }
    try {
        FinancialSummary summary = summaryInteractor.executeForMonth(userId, month);
        outputHandler.print("\n--- Summary for " + month + " ---");
        outputHandler.printSummary(summary);
    } catch (const std::exception& exception) {
        outputHandler.printError(exception.what());
    }
}
