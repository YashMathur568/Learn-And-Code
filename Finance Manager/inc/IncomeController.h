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
        IInputHandler& inputHandler,
        IOutputHandler& outputHandler
    );
    void handleAddIncome(int userId);
    void handleViewIncomes(int userId);
private:
    IAddIncomeInteractor& addInteractor;
    IGetIncomesInteractor& getInteractor;
    IInputHandler& inputHandler;
    IOutputHandler& outputHandler;
};
