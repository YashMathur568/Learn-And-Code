#include "SummaryController.h"

SummaryController::SummaryController(IGetSummaryInteractor& summaryInteractor, IOutputHandler& outputHandler)
    : summaryInteractor(summaryInteractor), outputHandler(outputHandler) {}

void SummaryController::handleShowSummary(int userId) {}
