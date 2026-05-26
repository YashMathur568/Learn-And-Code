#pragma once

#include "IExpenseRepository.h"
#include <algorithm>
#include <vector>

class MockExpenseRepository : public IExpenseRepository {
public:
    std::vector<Expense> expenses;
    bool addCalled = false;
    bool deleteByIdCalled = false;
    int deletedId = -1;

    void add(const Expense& expense) override {
        addCalled = true;
        expenses.push_back(expense);
    }

    std::vector<Expense> findAll(int userId) override {
        std::vector<Expense> result;
        for (const Expense& e : expenses)
            if (e.userId == userId) result.push_back(e);
        return result;
    }

    std::vector<Expense> findByCategory(int userId, Category category) override {
        std::vector<Expense> result;
        for (const Expense& e : expenses)
            if (e.userId == userId && e.category == category) result.push_back(e);
        return result;
    }

    std::vector<Expense> findByDateRange(int userId, const std::string& from, const std::string& to) override {
        std::vector<Expense> result;
        for (const Expense& e : expenses)
            if (e.userId == userId && e.date >= from && e.date <= to) result.push_back(e);
        return result;
    }

    void deleteById(int id) override {
        deleteByIdCalled = true;
        deletedId = id;
        expenses.erase(std::remove_if(expenses.begin(), expenses.end(),
            [id](const Expense& e) { return e.id == id; }), expenses.end());
    }
};
