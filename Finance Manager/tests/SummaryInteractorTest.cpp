#include <gtest/gtest.h>
#include "GetSummaryInteractor.h"
#include "../mocks/MockIncomeRepository.h"
#include "../mocks/MockExpenseRepository.h"

static Income makeIncome(int userId, double amount, const std::string& date) {
    Income i; i.id = 0; i.userId = userId; i.amount = amount;
    i.source = "Test"; i.description = "Test"; i.date = date;
    return i;
}

static Expense makeExpense(int userId, double amount, Category cat, const std::string& date) {
    Expense e; e.id = 0; e.userId = userId; e.amount = amount;
    e.category = cat; e.description = "Test"; e.date = date;
    return e;
}

// ─── GetSummaryInteractor (all-time) ─────────────────────────────────────────

TEST(GetSummaryInteractor, CorrectTotals) {
    MockIncomeRepository incomeRepo;
    MockExpenseRepository expenseRepo;
    incomeRepo.incomes.push_back(makeIncome(1, 5000.0, "2026-05-01"));
    incomeRepo.incomes.push_back(makeIncome(1, 3000.0, "2026-04-01"));
    expenseRepo.expenses.push_back(makeExpense(1, 1000.0, Category::FOOD,      "2026-05-01"));
    expenseRepo.expenses.push_back(makeExpense(1,  500.0, Category::TRANSPORT, "2026-04-10"));
    GetSummaryInteractor interactor(incomeRepo, expenseRepo);
    FinancialSummary summary = interactor.execute(1);
    EXPECT_DOUBLE_EQ(summary.totalIncome,   8000.0);
    EXPECT_DOUBLE_EQ(summary.totalExpenses, 1500.0);
    EXPECT_DOUBLE_EQ(summary.balance,       6500.0);
}

TEST(GetSummaryInteractor, CategoryBreakdownIsCorrect) {
    MockIncomeRepository incomeRepo;
    MockExpenseRepository expenseRepo;
    expenseRepo.expenses.push_back(makeExpense(1, 1000.0, Category::FOOD,      "2026-05-01"));
    expenseRepo.expenses.push_back(makeExpense(1,  600.0, Category::FOOD,      "2026-05-05"));
    expenseRepo.expenses.push_back(makeExpense(1,  400.0, Category::TRANSPORT, "2026-05-10"));
    GetSummaryInteractor interactor(incomeRepo, expenseRepo);
    FinancialSummary summary = interactor.execute(1);
    EXPECT_DOUBLE_EQ(summary.categoryBreakdown[Category::FOOD],      1600.0);
    EXPECT_DOUBLE_EQ(summary.categoryBreakdown[Category::TRANSPORT],  400.0);
}

TEST(GetSummaryInteractor, NegativeBalanceWhenExpensesExceedIncome) {
    MockIncomeRepository incomeRepo;
    MockExpenseRepository expenseRepo;
    incomeRepo.incomes.push_back(makeIncome(1, 1000.0, "2026-05-01"));
    expenseRepo.expenses.push_back(makeExpense(1, 2000.0, Category::FOOD, "2026-05-01"));
    GetSummaryInteractor interactor(incomeRepo, expenseRepo);
    FinancialSummary summary = interactor.execute(1);
    EXPECT_LT(summary.balance, 0.0);
    EXPECT_DOUBLE_EQ(summary.balance, -1000.0);
}

TEST(GetSummaryInteractor, EmptyReturnsZeros) {
    MockIncomeRepository incomeRepo;
    MockExpenseRepository expenseRepo;
    GetSummaryInteractor interactor(incomeRepo, expenseRepo);
    FinancialSummary summary = interactor.execute(1);
    EXPECT_DOUBLE_EQ(summary.totalIncome,   0.0);
    EXPECT_DOUBLE_EQ(summary.totalExpenses, 0.0);
    EXPECT_DOUBLE_EQ(summary.balance,       0.0);
    EXPECT_TRUE(summary.categoryBreakdown.empty());
}

TEST(GetSummaryInteractor, OnlyCountsCurrentUsersData) {
    MockIncomeRepository incomeRepo;
    MockExpenseRepository expenseRepo;
    incomeRepo.incomes.push_back(makeIncome(1, 5000.0, "2026-05-01"));
    incomeRepo.incomes.push_back(makeIncome(2, 9000.0, "2026-05-01"));
    expenseRepo.expenses.push_back(makeExpense(1, 1000.0, Category::FOOD, "2026-05-01"));
    expenseRepo.expenses.push_back(makeExpense(2, 4000.0, Category::FOOD, "2026-05-01"));
    GetSummaryInteractor interactor(incomeRepo, expenseRepo);
    FinancialSummary summary = interactor.execute(1);
    EXPECT_DOUBLE_EQ(summary.totalIncome,   5000.0);
    EXPECT_DOUBLE_EQ(summary.totalExpenses, 1000.0);
}

// ─── GetSummaryInteractor::executeForMonth ────────────────────────────────────

TEST(GetSummaryInteractor, MonthlyFilterOnlyCountsTargetMonth) {
    MockIncomeRepository incomeRepo;
    MockExpenseRepository expenseRepo;
    incomeRepo.incomes.push_back(makeIncome(1, 5000.0, "2026-05-01"));
    incomeRepo.incomes.push_back(makeIncome(1, 4000.0, "2026-04-01"));
    expenseRepo.expenses.push_back(makeExpense(1, 1000.0, Category::FOOD, "2026-05-10"));
    expenseRepo.expenses.push_back(makeExpense(1,  800.0, Category::FOOD, "2026-04-15"));
    GetSummaryInteractor interactor(incomeRepo, expenseRepo);
    FinancialSummary summary = interactor.executeForMonth(1, "2026-05");
    EXPECT_DOUBLE_EQ(summary.totalIncome,   5000.0);
    EXPECT_DOUBLE_EQ(summary.totalExpenses, 1000.0);
    EXPECT_DOUBLE_EQ(summary.balance,       4000.0);
}

TEST(GetSummaryInteractor, MonthlyFilterEmptyWhenNoDataInMonth) {
    MockIncomeRepository incomeRepo;
    MockExpenseRepository expenseRepo;
    incomeRepo.incomes.push_back(makeIncome(1, 5000.0, "2026-04-01"));
    expenseRepo.expenses.push_back(makeExpense(1, 1000.0, Category::FOOD, "2026-04-15"));
    GetSummaryInteractor interactor(incomeRepo, expenseRepo);
    FinancialSummary summary = interactor.executeForMonth(1, "2026-05");
    EXPECT_DOUBLE_EQ(summary.totalIncome,   0.0);
    EXPECT_DOUBLE_EQ(summary.totalExpenses, 0.0);
    EXPECT_DOUBLE_EQ(summary.balance,       0.0);
}

TEST(GetSummaryInteractor, MonthlyFilterMatchesExactPrefix) {
    MockIncomeRepository incomeRepo;
    MockExpenseRepository expenseRepo;
    incomeRepo.incomes.push_back(makeIncome(1, 2000.0, "2026-05-31"));
    incomeRepo.incomes.push_back(makeIncome(1, 1000.0, "2026-05-01"));
    GetSummaryInteractor interactor(incomeRepo, expenseRepo);
    FinancialSummary summary = interactor.executeForMonth(1, "2026-05");
    EXPECT_DOUBLE_EQ(summary.totalIncome, 3000.0);
}
