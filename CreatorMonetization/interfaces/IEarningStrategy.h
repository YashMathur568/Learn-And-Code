#ifndef IEARNING_STRATEGY_H
#define IEARNING_STRATEGY_H

#include <string>

class IEarningStrategy
{
public:
    virtual double calculateEarnings() const = 0;
    virtual std::string getStrategyName() const = 0;
    virtual ~IEarningStrategy() = default;
};

#endif
