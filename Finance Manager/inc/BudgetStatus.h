#pragma once

#include "Category.h"

class BudgetStatus {
public:
    Category category;
    double limitAmount;
    double spentAmount;
    double remainingAmount;
    bool isExceeded;
};
