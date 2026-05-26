#pragma once

#include <string>
#include "FinancialSummary.h"

class IGetSummaryInteractor {
public:
    virtual ~IGetSummaryInteractor() = default;
    virtual FinancialSummary execute(int userId) = 0;
    virtual FinancialSummary executeForMonth(int userId, const std::string& month) = 0;
};
