#pragma once

#include "Category.h"

struct BudgetStatus {
    Category category;
    double limitAmount;
    double spentAmount;
    double remainingAmount;
    bool isExceeded;
};
