#ifndef STATISTICS_CALCULATOR_H
#define STATISTICS_CALCULATOR_H

#include "IStatisticsCalculator.h"

class StatisticsCalculator : public IStatisticsCalculator
{
private:
    int totalRecords;
    double totalValue;
    double averageValue;

public:
    void calculate(const std::vector<Record>& records) override;
    int getTotalRecords() const override;
    double getTotalValue() const override;
    double getAverageValue() const override;
};

#endif
