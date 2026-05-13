#pragma once

#include <string>
#include "Category.h"

class Expense {
public:
    int id;
    int userId;
    double amount;
    Category category;
    std::string description;
    std::string date;
};
