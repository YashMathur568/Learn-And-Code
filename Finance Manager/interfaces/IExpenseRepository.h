#pragma once

#include <vector>
#include <string>
#include "Expense.h"
#include "Category.h"

class IExpenseRepository {
public:
    virtual ~IExpenseRepository() = default;
    virtual void add(const Expense& expense) = 0;
    virtual std::vector<Expense> findAll(int userId) = 0;
    virtual std::vector<Expense> findByCategory(int userId, Category category) = 0;
    virtual std::vector<Expense> findByDateRange(int userId, const std::string& from, const std::string& to) = 0;
    virtual void deleteById(int id) = 0;
};
