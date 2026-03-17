#include "StatisticsCalculator.h"

StatisticsCalculator::StatisticsCalculator()
    : totalRecords(0), errorCount(0), totalValue(0.0), averageValue(0.0)
{
}

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

void StatisticsCalculator::setErrorCount(int count)
{
    errorCount = count;
}

int StatisticsCalculator::getTotalRecords() const { return totalRecords; }
int StatisticsCalculator::getErrorCount() const { return errorCount; }
double StatisticsCalculator::getTotalValue() const { return totalValue; }
double StatisticsCalculator::getAverageValue() const { return averageValue; }
