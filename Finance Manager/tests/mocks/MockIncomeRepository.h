#pragma once

#include "IIncomeRepository.h"
#include <vector>

class MockIncomeRepository : public IIncomeRepository {
public:
    std::vector<Income> incomes;
    bool addCalled = false;

    void add(const Income& income) override {
        addCalled = true;
        incomes.push_back(income);
    }

    std::vector<Income> findAll(int userId) override {
        std::vector<Income> result;
        for (const Income& i : incomes)
            if (i.userId == userId) result.push_back(i);
        return result;
    }
};
