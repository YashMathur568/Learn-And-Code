#include "AdRevenueStrategy.h"

AdRevenueStrategy::AdRevenueStrategy(int viewCount, double ratePerView,
                                     double engagementRate, double regionMultiplier,
                                     double seasonMultiplier)
    : viewCount(viewCount),
      ratePerView(ratePerView),
      engagementRate(engagementRate),
      regionMultiplier(regionMultiplier),
      seasonMultiplier(seasonMultiplier)
{
}

double AdRevenueStrategy::calculateEarnings() const
{
    return viewCount * ratePerView * engagementRate * regionMultiplier * seasonMultiplier;
}

std::string AdRevenueStrategy::getStrategyName() const
{
    return "Ad Revenue";
}
