#include "SummaryController.h"

SummaryController::SummaryController(IGetSummaryInteractor& summaryInteractor, IOutputHandler& output)
    : summaryInteractor(summaryInteractor), output(output) {}

void SummaryController::handleShowSummary(int userId) {  }
