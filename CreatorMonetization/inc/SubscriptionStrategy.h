#ifndef SUBSCRIPTION_STRATEGY_H
#define SUBSCRIPTION_STRATEGY_H

#include "IEarningStrategy.h"

class SubscriptionStrategy : public IEarningStrategy
{
private:
    int subscriberCount;
    double pricePerSubscriber;

public:
    SubscriptionStrategy(int subscriberCount, double pricePerSubscriber);

    double calculateEarnings() const override;
    std::string getStrategyName() const override;
};

#endif
