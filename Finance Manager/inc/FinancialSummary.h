#pragma once

#include <map>
#include "Category.h"

class FinancialSummary {
public:
    double totalIncome;
    double totalExpenses;
    double balance;
    std::map<Category, double> categoryBreakdown;
};
