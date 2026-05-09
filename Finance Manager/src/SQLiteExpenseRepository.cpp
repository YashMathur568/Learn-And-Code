#include "SQLiteExpenseRepository.h"

SQLiteExpenseRepository::SQLiteExpenseRepository(IDatabaseConnection& db) : db(db) {}

void SQLiteExpenseRepository::add(const Expense& expense)                                                          {  }
std::vector<Expense> SQLiteExpenseRepository::findAll(int userId)                                                  {  return {}; }
std::vector<Expense> SQLiteExpenseRepository::findByCategory(int userId, Category category)                       {  return {}; }
std::vector<Expense> SQLiteExpenseRepository::findByDateRange(int userId, const std::string& from, const std::string& to) {  return {}; }
void SQLiteExpenseRepository::deleteById(int id)                                                                   {  }
