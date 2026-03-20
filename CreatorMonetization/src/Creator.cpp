#include "Creator.h"
#include <iostream>

Creator::Creator(const std::string& creatorName)
    : name(creatorName)
{
}

std::string Creator::getName() const
{
    return name;
}

void Creator::addEarningStrategy(const std::shared_ptr<IEarningStrategy>& strategy)
{
    earningStrategies.push_back(strategy);
}

double Creator::calculateTotalEarnings() const
{
    double totalEarnings = 0.0;
    for (const auto& strategy : earningStrategies)
    {
        totalEarnings += strategy->calculateEarnings();
    }
    return totalEarnings;
}

void Creator::displayEarnings() const
{
    std::cout << "Creator: " << name << std::endl;
    std::cout << "Earning Breakdown:" << std::endl;
    for (const auto& strategy : earningStrategies)
    {
        std::cout << "  " << strategy->getStrategyName() << ": $"
                  << strategy->calculateEarnings() << std::endl;
    }
    std::cout << "Total Earnings: $" << calculateTotalEarnings() << std::endl;
}
