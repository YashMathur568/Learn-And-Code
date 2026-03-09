#ifndef ISTATISTICS_CALCULATOR_H
#define ISTATISTICS_CALCULATOR_H

#include <vector>
#include "Record.h"

class IStatisticsCalculator
{
public:
    virtual ~IStatisticsCalculator() {}

    virtual void calculate(const std::vector<Record>& records) = 0;
    virtual void setErrorCount(int count) = 0;

    virtual int getTotalRecords() const = 0;
    virtual int getErrorCount() const = 0;
    virtual double getTotalValue() const = 0;
    virtual double getAverageValue() const = 0;
};

#endif
