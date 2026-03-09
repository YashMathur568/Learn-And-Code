#ifndef STATISTICS_CALCULATOR_H
#define STATISTICS_CALCULATOR_H

#include "IStatisticsCalculator.h"

class StatisticsCalculator : public IStatisticsCalculator
{
private:
    int totalRecords;
    int errorCount;
    double totalValue;
    double averageValue;

public:
    StatisticsCalculator();

    void calculate(const std::vector<Record>& records) override;
    void setErrorCount(int count) override;

    int getTotalRecords() const override;
    int getErrorCount() const override;
    double getTotalValue() const override;
    double getAverageValue() const override;
};

#endif
