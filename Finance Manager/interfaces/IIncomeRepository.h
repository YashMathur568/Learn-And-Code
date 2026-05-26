#pragma once

#include <vector>
#include "Income.h"

class IIncomeRepository {
public:
    virtual ~IIncomeRepository() = default;
    virtual void add(const Income& income) = 0;
    virtual std::vector<Income> findAll(int userId) = 0;
};
