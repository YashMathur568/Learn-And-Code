#include "IncomeController.h"

IncomeController::IncomeController(
    IAddIncomeInteractor& addInteractor,
    IGetIncomesInteractor& getInteractor,
    IInputHandler& inputHandler,
    IOutputHandler& outputHandler)
    : addInteractor(addInteractor), getInteractor(getInteractor), inputHandler(inputHandler), outputHandler(outputHandler) {}

void IncomeController::handleAddIncome(int userId)   {}
void IncomeController::handleViewIncomes(int userId) {}
