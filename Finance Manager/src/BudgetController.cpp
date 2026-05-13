#include "BudgetController.h"

BudgetController::BudgetController(
    ISetBudgetInteractor& setInteractor,
    ITrackBudgetInteractor& trackInteractor,
    IInputHandler& inputHandler,
    IOutputHandler& outputHandler)
    : setInteractor(setInteractor), trackInteractor(trackInteractor), inputHandler(inputHandler), outputHandler(outputHandler) {}

void BudgetController::handleSetBudget(int userId)   {}
void BudgetController::handleTrackBudget(int userId) {}
