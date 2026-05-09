#include "BudgetController.h"

BudgetController::BudgetController(
    ISetBudgetInteractor& setInteractor,
    ITrackBudgetInteractor& trackInteractor,
    IInputHandler& input,
    IOutputHandler& output)
    : setInteractor(setInteractor), trackInteractor(trackInteractor), input(input), output(output) {}

void BudgetController::handleSetBudget(int userId)   {  }
void BudgetController::handleTrackBudget(int userId) {  }
