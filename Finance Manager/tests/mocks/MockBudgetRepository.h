#pragma once

#include "IBudgetRepository.h"
#include <stdexcept>
#include <vector>

class MockBudgetRepository : public IBudgetRepository {
public:
    std::vector<Budget> budgets;
    bool saveCalled = false;

    void save(const Budget& budget) override {
        saveCalled = true;
        budgets.push_back(budget);
    }

    Budget findByCategory(int userId, Category category, const std::string& month) override {
        for (const Budget& b : budgets)
            if (b.userId == userId && b.category == category && b.month == month) return b;
        throw std::runtime_error("Budget not found");
    }

    std::vector<Budget> findAll(int userId) override {
        std::vector<Budget> result;
        for (const Budget& b : budgets)
            if (b.userId == userId) result.push_back(b);
        return result;
    }
};
