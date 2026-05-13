#include "SQLiteIncomeRepository.h"

SQLiteIncomeRepository::SQLiteIncomeRepository(IDatabaseConnection& databaseConnection) : databaseConnection(databaseConnection) {}

void SQLiteIncomeRepository::add(const Income& income)            {}
std::vector<Income> SQLiteIncomeRepository::findAll(int userId)   { return {}; }
