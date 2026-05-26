#pragma once

#include "IGetSummaryInteractor.h"
#include "IInputHandler.h"
#include "IOutputHandler.h"

class SummaryController {
public:
    SummaryController(IGetSummaryInteractor& summaryInteractor, IInputHandler& inputHandler, IOutputHandler& outputHandler);
    void handleShowSummary(int userId);
    void handleShowMonthlySummary(int userId);
private:
    IGetSummaryInteractor& summaryInteractor;
    IInputHandler& inputHandler;
    IOutputHandler& outputHandler;
};
