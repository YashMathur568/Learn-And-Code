#include <gtest/gtest.h>
#include "SQLiteConnection.h"
#include "SQLiteUserRepository.h"
#include "SQLiteSessionRepository.h"
#include "SQLiteExpenseRepository.h"
#include "SQLiteIncomeRepository.h"
#include "SQLiteBudgetRepository.h"
#include "AuthUtils.h"

class SQLiteRepoTest : public ::testing::Test {
protected:
    SQLiteConnection db{":memory:"};
};

// ─── SQLiteUserRepository ─────────────────────────────────────────────────────

TEST_F(SQLiteRepoTest, UserSaveAndFindByUsername) {
    SQLiteUserRepository repo(db);
    User user;
    user.username     = "alice";
    user.passwordHash = hashPassword("pass", "salt1");
    user.salt         = "salt1";
    user.createdAt    = getCurrentTimestamp();
    repo.save(user);
    User found = repo.findByUsername("alice");
    EXPECT_EQ(found.username,     "alice");
    EXPECT_EQ(found.passwordHash, user.passwordHash);
    EXPECT_EQ(found.salt,         "salt1");
    EXPECT_GT(found.userId, 0);
}

TEST_F(SQLiteRepoTest, UserExistsByUsername) {
    SQLiteUserRepository repo(db);
    EXPECT_FALSE(repo.existsByUsername("bob"));
    User user; user.username = "bob"; user.passwordHash = "h"; user.salt = "s"; user.createdAt = getCurrentTimestamp();
    repo.save(user);
    EXPECT_TRUE(repo.existsByUsername("bob"));
}

TEST_F(SQLiteRepoTest, UserFindByUsernameThrowsWhenNotFound) {
    SQLiteUserRepository repo(db);
    EXPECT_THROW(repo.findByUsername("nobody"), std::runtime_error);
}

// ─── SQLiteSessionRepository ──────────────────────────────────────────────────

TEST_F(SQLiteRepoTest, SessionSaveAndFindByToken) {
    SQLiteSessionRepository repo(db);
    Session session;
    session.userId    = 1;
    session.token     = "tok_abc123";
    session.expiresAt = getExpiryTimestamp(24);
    repo.save(session);
    Session found = repo.findByToken("tok_abc123");
    EXPECT_EQ(found.userId,    1);
    EXPECT_EQ(found.token,     "tok_abc123");
    EXPECT_EQ(found.expiresAt, session.expiresAt);
}

TEST_F(SQLiteRepoTest, SessionFindByTokenThrowsWhenNotFound) {
    SQLiteSessionRepository repo(db);
    EXPECT_THROW(repo.findByToken("no_such_token"), std::runtime_error);
}

TEST_F(SQLiteRepoTest, SessionDeleteByToken) {
    SQLiteSessionRepository repo(db);
    Session s; s.userId = 1; s.token = "del_tok"; s.expiresAt = getExpiryTimestamp(1);
    repo.save(s);
    repo.deleteByToken("del_tok");
    EXPECT_THROW(repo.findByToken("del_tok"), std::runtime_error);
}

TEST_F(SQLiteRepoTest, SessionDeleteByUserId) {
    SQLiteSessionRepository repo(db);
    Session s1; s1.userId = 5; s1.token = "t1"; s1.expiresAt = getExpiryTimestamp(1);
    Session s2; s2.userId = 5; s2.token = "t2"; s2.expiresAt = getExpiryTimestamp(1);
    Session s3; s3.userId = 9; s3.token = "t3"; s3.expiresAt = getExpiryTimestamp(1);
    repo.save(s1); repo.save(s2); repo.save(s3);
    repo.deleteByUserId(5);
    EXPECT_THROW(repo.findByToken("t1"), std::runtime_error);
    EXPECT_THROW(repo.findByToken("t2"), std::runtime_error);
    EXPECT_NO_THROW(repo.findByToken("t3"));
}

// ─── SQLiteExpenseRepository ──────────────────────────────────────────────────

TEST_F(SQLiteRepoTest, ExpenseAddAndFindAll) {
    SQLiteExpenseRepository repo(db);
    Expense e; e.userId = 1; e.amount = 250.0; e.category = Category::FOOD;
    e.description = "Groceries"; e.date = "2026-05-10";
    repo.add(e);
    auto results = repo.findAll(1);
    ASSERT_EQ(results.size(), 1u);
    EXPECT_DOUBLE_EQ(results[0].amount, 250.0);
    EXPECT_EQ(results[0].description,   "Groceries");
    EXPECT_EQ(results[0].category,      Category::FOOD);
}

TEST_F(SQLiteRepoTest, ExpenseFindAllIsolatedByUser) {
    SQLiteExpenseRepository repo(db);
    Expense e1; e1.userId = 1; e1.amount = 100.0; e1.category = Category::FOOD;
    e1.description = "User1"; e1.date = "2026-05-01";
    Expense e2; e2.userId = 2; e2.amount = 200.0; e2.category = Category::FOOD;
    e2.description = "User2"; e2.date = "2026-05-01";
    repo.add(e1); repo.add(e2);
    EXPECT_EQ(repo.findAll(1).size(), 1u);
    EXPECT_EQ(repo.findAll(2).size(), 1u);
}

TEST_F(SQLiteRepoTest, ExpenseFindByCategory) {
    SQLiteExpenseRepository repo(db);
    Expense e1; e1.userId = 1; e1.amount = 100.0; e1.category = Category::FOOD;
    e1.description = "Food"; e1.date = "2026-05-01";
    Expense e2; e2.userId = 1; e2.amount = 50.0; e2.category = Category::TRANSPORT;
    e2.description = "Bus"; e2.date = "2026-05-02";
    repo.add(e1); repo.add(e2);
    auto food = repo.findByCategory(1, Category::FOOD);
    ASSERT_EQ(food.size(), 1u);
    EXPECT_EQ(food[0].description, "Food");
    EXPECT_TRUE(repo.findByCategory(1, Category::HEALTH).empty());
}

TEST_F(SQLiteRepoTest, ExpenseFindByDateRange) {
    SQLiteExpenseRepository repo(db);
    Expense e1; e1.userId = 1; e1.amount = 100.0; e1.category = Category::FOOD;
    e1.description = "Early"; e1.date = "2026-04-01";
    Expense e2; e2.userId = 1; e2.amount = 200.0; e2.category = Category::FOOD;
    e2.description = "Target"; e2.date = "2026-05-15";
    Expense e3; e3.userId = 1; e3.amount = 300.0; e3.category = Category::FOOD;
    e3.description = "Late"; e3.date = "2026-06-01";
    repo.add(e1); repo.add(e2); repo.add(e3);
    auto results = repo.findByDateRange(1, "2026-05-01", "2026-05-31");
    ASSERT_EQ(results.size(), 1u);
    EXPECT_EQ(results[0].description, "Target");
}

TEST_F(SQLiteRepoTest, ExpenseDeleteById) {
    SQLiteExpenseRepository repo(db);
    Expense e; e.userId = 1; e.amount = 100.0; e.category = Category::FOOD;
    e.description = "ToDelete"; e.date = "2026-05-01";
    repo.add(e);
    auto before = repo.findAll(1);
    ASSERT_EQ(before.size(), 1u);
    repo.deleteById(before[0].expenseId);
    EXPECT_TRUE(repo.findAll(1).empty());
}

// ─── SQLiteIncomeRepository ───────────────────────────────────────────────────

TEST_F(SQLiteRepoTest, IncomeAddAndFindAll) {
    SQLiteIncomeRepository repo(db);
    Income i; i.userId = 1; i.amount = 5000.0; i.source = "Salary";
    i.description = "Monthly"; i.date = "2026-05-01";
    repo.add(i);
    auto results = repo.findAll(1);
    ASSERT_EQ(results.size(), 1u);
    EXPECT_DOUBLE_EQ(results[0].amount, 5000.0);
    EXPECT_EQ(results[0].source, "Salary");
}

TEST_F(SQLiteRepoTest, IncomeFindAllIsolatedByUser) {
    SQLiteIncomeRepository repo(db);
    Income i1; i1.userId = 1; i1.amount = 1000.0; i1.source = "A";
    i1.description = "D"; i1.date = "2026-05-01";
    Income i2; i2.userId = 2; i2.amount = 2000.0; i2.source = "B";
    i2.description = "D"; i2.date = "2026-05-01";
    repo.add(i1); repo.add(i2);
    EXPECT_EQ(repo.findAll(1).size(), 1u);
    EXPECT_EQ(repo.findAll(2).size(), 1u);
}

// ─── SQLiteBudgetRepository ───────────────────────────────────────────────────

TEST_F(SQLiteRepoTest, BudgetSaveAndFindByCategory) {
    SQLiteBudgetRepository repo(db);
    Budget b; b.userId = 1; b.category = Category::FOOD;
    b.limitAmount = 3000.0; b.month = "2026-05";
    repo.save(b);
    Budget found = repo.findByCategory(1, Category::FOOD, "2026-05");
    EXPECT_DOUBLE_EQ(found.limitAmount, 3000.0);
    EXPECT_EQ(found.month,    "2026-05");
    EXPECT_EQ(found.category, Category::FOOD);
}

TEST_F(SQLiteRepoTest, BudgetFindByCategoryThrowsWhenNotFound) {
    SQLiteBudgetRepository repo(db);
    EXPECT_THROW(repo.findByCategory(1, Category::HEALTH, "2026-05"), std::runtime_error);
}

TEST_F(SQLiteRepoTest, BudgetFindAll) {
    SQLiteBudgetRepository repo(db);
    Budget b1; b1.userId = 1; b1.category = Category::FOOD;
    b1.limitAmount = 1000.0; b1.month = "2026-05";
    Budget b2; b2.userId = 1; b2.category = Category::TRANSPORT;
    b2.limitAmount = 500.0; b2.month = "2026-05";
    repo.save(b1); repo.save(b2);
    EXPECT_EQ(repo.findAll(1).size(), 2u);
}

TEST_F(SQLiteRepoTest, BudgetSaveReplacesExistingForSameCategoryAndMonth) {
    SQLiteBudgetRepository repo(db);
    Budget b; b.userId = 1; b.category = Category::FOOD; b.limitAmount = 1000.0; b.month = "2026-05";
    repo.save(b);
    b.limitAmount = 2500.0;
    repo.save(b);
    Budget found = repo.findByCategory(1, Category::FOOD, "2026-05");
    EXPECT_DOUBLE_EQ(found.limitAmount, 2500.0);
    EXPECT_EQ(repo.findAll(1).size(), 1u);
}

TEST_F(SQLiteRepoTest, BudgetDifferentMonthsAreIndependent) {
    SQLiteBudgetRepository repo(db);
    Budget b1; b1.userId = 1; b1.category = Category::FOOD; b1.limitAmount = 1000.0; b1.month = "2026-05";
    Budget b2; b2.userId = 1; b2.category = Category::FOOD; b2.limitAmount = 1500.0; b2.month = "2026-06";
    repo.save(b1); repo.save(b2);
    EXPECT_DOUBLE_EQ(repo.findByCategory(1, Category::FOOD, "2026-05").limitAmount, 1000.0);
    EXPECT_DOUBLE_EQ(repo.findByCategory(1, Category::FOOD, "2026-06").limitAmount, 1500.0);
    EXPECT_EQ(repo.findAll(1).size(), 2u);
}
