#include "DeleteExpenseInteractor.h"

DeleteExpenseInteractor::DeleteExpenseInteractor(IExpenseRepository& expenseRepository)
    : expenseRepository(expenseRepository) {}

void DeleteExpenseInteractor::execute(int id) {
    expenseRepository.deleteById(id);
}
