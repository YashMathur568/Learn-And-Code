#include "GetSummaryInteractor.h"

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
