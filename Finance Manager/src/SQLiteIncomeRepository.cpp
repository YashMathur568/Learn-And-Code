#include "SQLiteIncomeRepository.h"

SQLiteIncomeRepository::SQLiteIncomeRepository(IDatabaseConnection& db) : db(db) {}

void SQLiteIncomeRepository::add(const Income& income)            {  }
std::vector<Income> SQLiteIncomeRepository::findAll(int userId)   {  return {}; }
