#pragma once

#include "IAddIncomeInteractor.h"
#include "IGetIncomesInteractor.h"
#include "IInputHandler.h"
#include "IOutputHandler.h"

class IncomeController {
public:
    IncomeController(
        IAddIncomeInteractor& addInteractor,
        IGetIncomesInteractor& getInteractor,
        IInputHandler& input,
        IOutputHandler& output
    );
    void handleAddIncome(int userId);
    void handleViewIncomes(int userId);
private:
    IAddIncomeInteractor& addInteractor;
    IGetIncomesInteractor& getInteractor;
    IInputHandler& input;
    IOutputHandler& output;
};
