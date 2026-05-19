#include "SummaryController.h"
#include <stdexcept>

SummaryController::SummaryController(IGetSummaryInteractor& summaryInteractor, IOutputHandler& outputHandler)
    : summaryInteractor(summaryInteractor), outputHandler(outputHandler) {}

void SummaryController::handleShowSummary(int userId) {
    try {
        FinancialSummary summary = summaryInteractor.execute(userId);
        outputHandler.printSummary(summary);
    } catch (const std::exception& exception) {
        outputHandler.printError(exception.what());
    }
}
