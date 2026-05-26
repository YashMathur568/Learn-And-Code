#include <gtest/gtest.h>
#include "AddIncomeInteractor.h"
#include "GetIncomesInteractor.h"
#include "SetBudgetInteractor.h"
#include "TrackBudgetInteractor.h"
#include "../mocks/MockIncomeRepository.h"
#include "../mocks/MockExpenseRepository.h"
#include "../mocks/MockBudgetRepository.h"

static Income makeIncome(int userId, double amount, const std::string& source,
                          const std::string& desc, const std::string& date) {
    Income i;
    i.incomeId = 0; i.userId = userId; i.amount = amount;
    i.source = source; i.description = desc; i.date = date;
    return i;
}

static Budget makeBudget(int userId, Category cat, double limit, const std::string& month) {
    Budget b;
    b.budgetId = 0; b.userId = userId; b.category = cat;
    b.limitAmount = limit; b.month = month;
    return b;
}

// ─── AddIncomeInteractor ──────────────────────────────────────────────────────

TEST(AddIncomeInteractor, ValidIncomeIsAdded) {
    MockIncomeRepository repo;
    AddIncomeInteractor interactor(repo);
    interactor.execute(makeIncome(1, 5000.0, "Salary", "Monthly", "2026-05-01"));
    EXPECT_TRUE(repo.addCalled);
    EXPECT_EQ(repo.incomes.size(), 1u);
}

TEST(AddIncomeInteractor, ZeroAmountThrows) {
    MockIncomeRepository repo;
    AddIncomeInteractor interactor(repo);
    EXPECT_THROW(interactor.execute(makeIncome(1, 0.0, "Salary", "Monthly", "2026-05-01")), std::runtime_error);
}

TEST(AddIncomeInteractor, NegativeAmountThrows) {
    MockIncomeRepository repo;
    AddIncomeInteractor interactor(repo);
    EXPECT_THROW(interactor.execute(makeIncome(1, -100.0, "Salary", "Monthly", "2026-05-01")), std::runtime_error);
}

TEST(AddIncomeInteractor, EmptySourceThrows) {
    MockIncomeRepository repo;
    AddIncomeInteractor interactor(repo);
    EXPECT_THROW(interactor.execute(makeIncome(1, 500.0, "", "Monthly", "2026-05-01")), std::runtime_error);
}

TEST(AddIncomeInteractor, EmptyDateThrows) {
    MockIncomeRepository repo;
    AddIncomeInteractor interactor(repo);
    EXPECT_THROW(interactor.execute(makeIncome(1, 500.0, "Salary", "Monthly", "")), std::runtime_error);
}

// ─── GetIncomesInteractor ─────────────────────────────────────────────────────

TEST(GetIncomesInteractor, ReturnsIncomesForUser) {
    MockIncomeRepository repo;
    repo.incomes.push_back(makeIncome(1, 5000.0, "Salary", "Monthly", "2026-05-01"));
    repo.incomes.push_back(makeIncome(2, 3000.0, "Freelance", "Project", "2026-05-10"));
    GetIncomesInteractor interactor(repo);
    auto result = interactor.execute(1);
    EXPECT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0].source, "Salary");
}

TEST(GetIncomesInteractor, ReturnsEmptyForUserWithNoIncome) {
    MockIncomeRepository repo;
    GetIncomesInteractor interactor(repo);
    EXPECT_TRUE(interactor.execute(99).empty());
}

// ─── SetBudgetInteractor ──────────────────────────────────────────────────────

TEST(SetBudgetInteractor, ValidBudgetIsSaved) {
    MockBudgetRepository repo;
    SetBudgetInteractor interactor(repo);
    interactor.execute(makeBudget(1, Category::FOOD, 3000.0, "2026-05"));
    EXPECT_TRUE(repo.saveCalled);
    EXPECT_EQ(repo.budgets.size(), 1u);
}

TEST(SetBudgetInteractor, ZeroLimitThrows) {
    MockBudgetRepository repo;
    SetBudgetInteractor interactor(repo);
    EXPECT_THROW(interactor.execute(makeBudget(1, Category::FOOD, 0.0, "2026-05")), std::runtime_error);
}

TEST(SetBudgetInteractor, NegativeLimitThrows) {
    MockBudgetRepository repo;
    SetBudgetInteractor interactor(repo);
    EXPECT_THROW(interactor.execute(makeBudget(1, Category::FOOD, -500.0, "2026-05")), std::runtime_error);
}

TEST(SetBudgetInteractor, EmptyMonthThrows) {
    MockBudgetRepository repo;
    SetBudgetInteractor interactor(repo);
    EXPECT_THROW(interactor.execute(makeBudget(1, Category::FOOD, 3000.0, "")), std::runtime_error);
}

// ─── TrackBudgetInteractor ────────────────────────────────────────────────────

TEST(TrackBudgetInteractor, UnderBudgetIsNotExceeded) {
    MockBudgetRepository budgetRepo;
    MockExpenseRepository expenseRepo;
    budgetRepo.budgets.push_back(makeBudget(1, Category::FOOD, 3000.0, "2026-05"));
    Expense e; e.expenseId = 1; e.userId = 1; e.amount = 1000.0;
    e.category = Category::FOOD; e.description = "Groceries"; e.date = "2026-05-10";
    expenseRepo.expenses.push_back(e);
    TrackBudgetInteractor interactor(budgetRepo, expenseRepo);
    BudgetStatus status = interactor.execute(1, Category::FOOD, "2026-05");
    EXPECT_DOUBLE_EQ(status.limitAmount, 3000.0);
    EXPECT_DOUBLE_EQ(status.spentAmount, 1000.0);
    EXPECT_DOUBLE_EQ(status.remainingAmount, 2000.0);
    EXPECT_FALSE(status.isExceeded);
}

TEST(TrackBudgetInteractor, OverBudgetIsExceeded) {
    MockBudgetRepository budgetRepo;
    MockExpenseRepository expenseRepo;
    budgetRepo.budgets.push_back(makeBudget(1, Category::FOOD, 500.0, "2026-05"));
    Expense e; e.expenseId = 1; e.userId = 1; e.amount = 800.0;
    e.category = Category::FOOD; e.description = "Big shop"; e.date = "2026-05-10";
    expenseRepo.expenses.push_back(e);
    TrackBudgetInteractor interactor(budgetRepo, expenseRepo);
    BudgetStatus status = interactor.execute(1, Category::FOOD, "2026-05");
    EXPECT_TRUE(status.isExceeded);
    EXPECT_DOUBLE_EQ(status.remainingAmount, -300.0);
}

TEST(TrackBudgetInteractor, ExactlyAtLimitIsNotExceeded) {
    MockBudgetRepository budgetRepo;
    MockExpenseRepository expenseRepo;
    budgetRepo.budgets.push_back(makeBudget(1, Category::FOOD, 1000.0, "2026-05"));
    Expense e; e.expenseId = 1; e.userId = 1; e.amount = 1000.0;
    e.category = Category::FOOD; e.description = "Exact"; e.date = "2026-05-10";
    expenseRepo.expenses.push_back(e);
    TrackBudgetInteractor interactor(budgetRepo, expenseRepo);
    BudgetStatus status = interactor.execute(1, Category::FOOD, "2026-05");
    EXPECT_FALSE(status.isExceeded);
    EXPECT_DOUBLE_EQ(status.remainingAmount, 0.0);
}

TEST(TrackBudgetInteractor, NoBudgetSetThrows) {
    MockBudgetRepository budgetRepo;
    MockExpenseRepository expenseRepo;
    TrackBudgetInteractor interactor(budgetRepo, expenseRepo);
    EXPECT_THROW(interactor.execute(1, Category::FOOD, "2026-05"), std::runtime_error);
}

TEST(TrackBudgetInteractor, ExpensesFromOtherMonthsAreExcluded) {
    MockBudgetRepository budgetRepo;
    MockExpenseRepository expenseRepo;
    budgetRepo.budgets.push_back(makeBudget(1, Category::FOOD, 1000.0, "2026-05"));
    Expense e1; e1.expenseId = 1; e1.userId = 1; e1.amount = 400.0;
    e1.category = Category::FOOD; e1.description = "May";  e1.date = "2026-05-10";
    Expense e2; e2.expenseId = 2; e2.userId = 1; e2.amount = 900.0;
    e2.category = Category::FOOD; e2.description = "April"; e2.date = "2026-04-15";
    expenseRepo.expenses.push_back(e1);
    expenseRepo.expenses.push_back(e2);
    TrackBudgetInteractor interactor(budgetRepo, expenseRepo);
    BudgetStatus status = interactor.execute(1, Category::FOOD, "2026-05");
    EXPECT_DOUBLE_EQ(status.spentAmount, 400.0);
    EXPECT_FALSE(status.isExceeded);
}
