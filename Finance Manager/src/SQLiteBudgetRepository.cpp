#include "SQLiteBudgetRepository.h"

SQLiteBudgetRepository::SQLiteBudgetRepository(IDatabaseConnection& db) : db(db) {}

void SQLiteBudgetRepository::save(const Budget& budget)                               {  }
Budget SQLiteBudgetRepository::findByCategory(int userId, Category category)          {  return {}; }
std::vector<Budget> SQLiteBudgetRepository::findAll(int userId)                       {  return {}; }
