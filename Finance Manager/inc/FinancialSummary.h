#pragma once

#include <map>
#include "Category.h"

struct FinancialSummary {
    double totalIncome;
    double totalExpenses;
    double balance;
    std::map<Category, double> categoryBreakdown;
};
