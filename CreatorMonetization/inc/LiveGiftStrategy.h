#ifndef LIVE_GIFT_STRATEGY_H
#define LIVE_GIFT_STRATEGY_H

#include "IEarningStrategy.h"

class LiveGiftStrategy : public IEarningStrategy
{
private:
    int giftCount;
    double averageGiftValue;

public:
    LiveGiftStrategy(int giftCount, double averageGiftValue);

    double calculateEarnings() const override;
    std::string getStrategyName() const override;
};

#endif
