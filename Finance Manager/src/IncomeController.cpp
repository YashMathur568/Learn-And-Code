#include "IncomeController.h"

IncomeController::IncomeController(
    IAddIncomeInteractor& addInteractor,
    IGetIncomesInteractor& getInteractor,
    IInputHandler& input,
    IOutputHandler& output)
    : addInteractor(addInteractor), getInteractor(getInteractor), input(input), output(output) {}

void IncomeController::handleAddIncome(int userId)   {  }
void IncomeController::handleViewIncomes(int userId) {  }
