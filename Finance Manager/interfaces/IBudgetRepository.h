#pragma once

#include <vector>
#include "Budget.h"
#include "Category.h"

class IBudgetRepository {
public:
    virtual ~IBudgetRepository() = default;
    virtual void save(const Budget& budget) = 0;
    virtual Budget findByCategory(int userId, Category category, const std::string& month) = 0;
    virtual std::vector<Budget> findAll(int userId) = 0;
};
