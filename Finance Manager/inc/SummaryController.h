#pragma once

#include "IGetSummaryInteractor.h"
#include "IOutputHandler.h"

class SummaryController {
public:
    SummaryController(IGetSummaryInteractor& summaryInteractor, IOutputHandler& output);
    void handleShowSummary(int userId);
private:
    IGetSummaryInteractor& summaryInteractor;
    IOutputHandler& output;
};
