#include "GetSummaryInteractor.h"
#include <algorithm>

GetSummaryInteractor::GetSummaryInteractor(IIncomeRepository& incomeRepository, IExpenseRepository& expenseRepository)
    : incomeRepository(incomeRepository), expenseRepository(expenseRepository) {}

FinancialSummary GetSummaryInteractor::execute(int userId) {
    std::vector<Income> incomes   = incomeRepository.findAll(userId);
    std::vector<Expense> expenses = expenseRepository.findAll(userId);
    FinancialSummary summary;
    summary.totalIncome   = 0.0;
    summary.totalExpenses = 0.0;
    for (const Income& income : incomes)
        summary.totalIncome += income.amount;
    for (const Expense& expense : expenses) {
        summary.totalExpenses += expense.amount;
        summary.categoryBreakdown[expense.category] += expense.amount;
    }
    summary.balance = summary.totalIncome - summary.totalExpenses;
    return summary;
}

FinancialSummary GetSummaryInteractor::executeForMonth(int userId, const std::string& month) {
    std::vector<Income> allIncomes   = incomeRepository.findAll(userId);
    std::vector<Expense> allExpenses = expenseRepository.findAll(userId);
    FinancialSummary summary;
    summary.totalIncome   = 0.0;
    summary.totalExpenses = 0.0;
    for (const Income& income : allIncomes) {
        if (income.date.size() >= 7 && income.date.substr(0, 7) == month)
            summary.totalIncome += income.amount;
    }
    for (const Expense& expense : allExpenses) {
        if (expense.date.size() >= 7 && expense.date.substr(0, 7) == month) {
            summary.totalExpenses += expense.amount;
            summary.categoryBreakdown[expense.category] += expense.amount;
        }
    }
    summary.balance = summary.totalIncome - summary.totalExpenses;
    return summary;
}
