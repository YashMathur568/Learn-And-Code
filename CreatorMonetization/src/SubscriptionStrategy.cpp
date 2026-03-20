#include "SubscriptionStrategy.h"

SubscriptionStrategy::SubscriptionStrategy(int subscriberCount, double pricePerSubscriber)
    : subscriberCount(subscriberCount),
      pricePerSubscriber(pricePerSubscriber)
{
}

double SubscriptionStrategy::calculateEarnings() const
{
    return subscriberCount * pricePerSubscriber;
}

std::string SubscriptionStrategy::getStrategyName() const
{
    return "Subscription";
}
