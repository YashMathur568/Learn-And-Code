#include <gtest/gtest.h>
#include "AddExpenseInteractor.h"
#include "GetExpensesInteractor.h"
#include "FilterExpensesByCategoryInteractor.h"
#include "FilterExpensesByDateInteractor.h"
#include "DeleteExpenseInteractor.h"
#include "../mocks/MockExpenseRepository.h"

static Expense makeExpense(int userId, double amount, Category cat,
                            const std::string& desc, const std::string& date, int id = 0) {
    Expense e;
    e.expenseId = id; e.userId = userId; e.amount = amount;
    e.category = cat; e.description = desc; e.date = date;
    return e;
}

// ─── AddExpenseInteractor ─────────────────────────────────────────────────────

TEST(AddExpenseInteractor, ValidExpenseIsAdded) {
    MockExpenseRepository repo;
    AddExpenseInteractor interactor(repo);
    Expense e = makeExpense(1, 100.0, Category::FOOD, "Lunch", "2026-05-01");
    interactor.execute(e);
    EXPECT_TRUE(repo.addCalled);
    EXPECT_EQ(repo.expenses.size(), 1u);
}

TEST(AddExpenseInteractor, ZeroAmountThrows) {
    MockExpenseRepository repo;
    AddExpenseInteractor interactor(repo);
    EXPECT_THROW(interactor.execute(makeExpense(1, 0.0, Category::FOOD, "X", "2026-05-01")), std::runtime_error);
}

TEST(AddExpenseInteractor, NegativeAmountThrows) {
    MockExpenseRepository repo;
    AddExpenseInteractor interactor(repo);
    EXPECT_THROW(interactor.execute(makeExpense(1, -50.0, Category::FOOD, "X", "2026-05-01")), std::runtime_error);
}

TEST(AddExpenseInteractor, EmptyDescriptionThrows) {
    MockExpenseRepository repo;
    AddExpenseInteractor interactor(repo);
    EXPECT_THROW(interactor.execute(makeExpense(1, 100.0, Category::FOOD, "", "2026-05-01")), std::runtime_error);
}

TEST(AddExpenseInteractor, EmptyDateThrows) {
    MockExpenseRepository repo;
    AddExpenseInteractor interactor(repo);
    EXPECT_THROW(interactor.execute(makeExpense(1, 100.0, Category::FOOD, "Lunch", "")), std::runtime_error);
}

// ─── GetExpensesInteractor ────────────────────────────────────────────────────

TEST(GetExpensesInteractor, ReturnsExpensesForUser) {
    MockExpenseRepository repo;
    repo.expenses.push_back(makeExpense(1, 100.0, Category::FOOD, "Lunch", "2026-05-01", 1));
    repo.expenses.push_back(makeExpense(2, 200.0, Category::TRANSPORT, "Bus", "2026-05-02", 2));
    GetExpensesInteractor interactor(repo);
    auto result = interactor.execute(1);
    EXPECT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0].description, "Lunch");
}

TEST(GetExpensesInteractor, ReturnsEmptyForUserWithNoExpenses) {
    MockExpenseRepository repo;
    GetExpensesInteractor interactor(repo);
    EXPECT_TRUE(interactor.execute(99).empty());
}

// ─── FilterExpensesByCategoryInteractor ──────────────────────────────────────

TEST(FilterExpensesByCategoryInteractor, FiltersCorrectly) {
    MockExpenseRepository repo;
    repo.expenses.push_back(makeExpense(1, 100.0, Category::FOOD,      "Lunch", "2026-05-01", 1));
    repo.expenses.push_back(makeExpense(1, 200.0, Category::TRANSPORT, "Bus",   "2026-05-02", 2));
    repo.expenses.push_back(makeExpense(1, 50.0,  Category::FOOD,      "Snack", "2026-05-03", 3));
    FilterExpensesByCategoryInteractor interactor(repo);
    auto result = interactor.execute(1, Category::FOOD);
    EXPECT_EQ(result.size(), 2u);
}

TEST(FilterExpensesByCategoryInteractor, ReturnsEmptyForUnmatchedCategory) {
    MockExpenseRepository repo;
    repo.expenses.push_back(makeExpense(1, 100.0, Category::FOOD, "Lunch", "2026-05-01", 1));
    FilterExpensesByCategoryInteractor interactor(repo);
    EXPECT_TRUE(interactor.execute(1, Category::HEALTH).empty());
}

// ─── FilterExpensesByDateInteractor ──────────────────────────────────────────

TEST(FilterExpensesByDateInteractor, FiltersWithinRange) {
    MockExpenseRepository repo;
    repo.expenses.push_back(makeExpense(1, 100.0, Category::FOOD, "A", "2026-05-01", 1));
    repo.expenses.push_back(makeExpense(1, 200.0, Category::FOOD, "B", "2026-05-10", 2));
    repo.expenses.push_back(makeExpense(1, 300.0, Category::FOOD, "C", "2026-05-20", 3));
    FilterExpensesByDateInteractor interactor(repo);
    auto result = interactor.execute(1, "2026-05-05", "2026-05-15");
    EXPECT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0].description, "B");
}

TEST(FilterExpensesByDateInteractor, InclusiveBoundaries) {
    MockExpenseRepository repo;
    repo.expenses.push_back(makeExpense(1, 100.0, Category::FOOD, "Start", "2026-05-01", 1));
    repo.expenses.push_back(makeExpense(1, 200.0, Category::FOOD, "End",   "2026-05-31", 2));
    FilterExpensesByDateInteractor interactor(repo);
    auto result = interactor.execute(1, "2026-05-01", "2026-05-31");
    EXPECT_EQ(result.size(), 2u);
}

TEST(FilterExpensesByDateInteractor, ReturnsEmptyWhenNoneInRange) {
    MockExpenseRepository repo;
    repo.expenses.push_back(makeExpense(1, 100.0, Category::FOOD, "Old", "2026-01-01", 1));
    FilterExpensesByDateInteractor interactor(repo);
    EXPECT_TRUE(interactor.execute(1, "2026-05-01", "2026-05-31").empty());
}

// ─── DeleteExpenseInteractor ──────────────────────────────────────────────────

TEST(DeleteExpenseInteractor, DeletesExpenseById) {
    MockExpenseRepository repo;
    repo.expenses.push_back(makeExpense(1, 100.0, Category::FOOD, "Lunch", "2026-05-01", 5));
    DeleteExpenseInteractor interactor(repo);
    interactor.execute(5);
    EXPECT_TRUE(repo.deleteByIdCalled);
    EXPECT_EQ(repo.deletedId, 5);
    EXPECT_TRUE(repo.expenses.empty());
}
