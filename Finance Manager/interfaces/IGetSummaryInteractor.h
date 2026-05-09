#pragma once

#include "FinancialSummary.h"

class IGetSummaryInteractor {
public:
    virtual ~IGetSummaryInteractor() = default;
    virtual FinancialSummary execute(int userId) = 0;
};
