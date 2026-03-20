#include "LiveGiftStrategy.h"

LiveGiftStrategy::LiveGiftStrategy(int giftCount, double averageGiftValue)
    : giftCount(giftCount),
      averageGiftValue(averageGiftValue)
{
}

double LiveGiftStrategy::calculateEarnings() const
{
    return giftCount * averageGiftValue;
}

std::string LiveGiftStrategy::getStrategyName() const
{
    return "Live Gift";
}
