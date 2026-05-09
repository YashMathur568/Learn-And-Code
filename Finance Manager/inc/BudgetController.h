#pragma once

#include "ISetBudgetInteractor.h"
#include "ITrackBudgetInteractor.h"
#include "IInputHandler.h"
#include "IOutputHandler.h"

class BudgetController {
public:
    BudgetController(
        ISetBudgetInteractor& setInteractor,
        ITrackBudgetInteractor& trackInteractor,
        IInputHandler& input,
        IOutputHandler& output
    );
    void handleSetBudget(int userId);
    void handleTrackBudget(int userId);
private:
    ISetBudgetInteractor& setInteractor;
    ITrackBudgetInteractor& trackInteractor;
    IInputHandler& input;
    IOutputHandler& output;
};
