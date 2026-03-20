#ifndef AD_REVENUE_STRATEGY_H
#define AD_REVENUE_STRATEGY_H

#include "IEarningStrategy.h"

class AdRevenueStrategy : public IEarningStrategy
{
private:
    int viewCount;
    double ratePerView;
    double engagementRate;
    double regionMultiplier;
    double seasonMultiplier;

public:
    AdRevenueStrategy(int viewCount, double ratePerView,
                      double engagementRate, double regionMultiplier,
                      double seasonMultiplier);

    double calculateEarnings() const override;
    std::string getStrategyName() const override;
};

#endif
