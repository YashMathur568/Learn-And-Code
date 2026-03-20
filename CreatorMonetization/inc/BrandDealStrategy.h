#ifndef BRAND_DEAL_STRATEGY_H
#define BRAND_DEAL_STRATEGY_H

#include "IEarningStrategy.h"

class BrandDealStrategy : public IEarningStrategy
{
private:
    double dealAmount;
    double engagementRate;
    double regionMultiplier;
    double seasonMultiplier;

public:
    BrandDealStrategy(double dealAmount, double engagementRate,
                      double regionMultiplier, double seasonMultiplier);

    double calculateEarnings() const override;
    std::string getStrategyName() const override;
};

#endif
