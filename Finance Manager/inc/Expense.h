#pragma once

#include <string>
#include "Category.h"

class Expense {
public:
    int expenseId;
    int userId;
    double amount;
    Category category;
    std::string description;
    std::string date;
};
