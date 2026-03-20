#include "BrandDealStrategy.h"

BrandDealStrategy::BrandDealStrategy(double dealAmount, double engagementRate,
                                     double regionMultiplier, double seasonMultiplier)
    : dealAmount(dealAmount),
      engagementRate(engagementRate),
      regionMultiplier(regionMultiplier),
      seasonMultiplier(seasonMultiplier)
{
}

double BrandDealStrategy::calculateEarnings() const
{
    return dealAmount * engagementRate * regionMultiplier * seasonMultiplier;
}

std::string BrandDealStrategy::getStrategyName() const
{
    return "Brand Deal";
}
