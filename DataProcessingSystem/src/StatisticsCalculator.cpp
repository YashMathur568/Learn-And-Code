#include "StatisticsCalculator.h"

void StatisticsCalculator::calculate(const std::vector<Record>& records)
{
    totalRecords = records.size();
    totalValue = 0;

    for (const auto& record : records)
        totalValue += record.getValue();

    averageValue = (totalRecords > 0)
        ? totalValue / totalRecords
        : 0;
}

int StatisticsCalculator::getTotalRecords() const { return totalRecords; }
double StatisticsCalculator::getTotalValue() const { return totalValue; }
double StatisticsCalculator::getAverageValue() const { return averageValue; }
