#pragma once

#include <string>
#include "Category.h"

struct Expense {
    int id;
    int userId;
    double amount;
    Category category;
    std::string description;
    std::string date;
};
