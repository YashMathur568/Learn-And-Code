#include "Creator.h"
#include "AdRevenueStrategy.h"
#include "SubscriptionStrategy.h"
#include "BrandDealStrategy.h"
#include "LiveGiftStrategy.h"
#include <iostream>
#include <memory>
#include <vector>

int main()
{
    std::cout << "=== Creator Monetization Platform ===" << std::endl << std::endl;

    std::vector<Creator> creators;

    creators.emplace_back("Yash");
    creators[0].addEarningStrategy(std::make_shared<AdRevenueStrategy>(100000, 0.05, 1.2, 1.1, 1.3));
    creators[0].addEarningStrategy(std::make_shared<SubscriptionStrategy>(5000, 2.0));
    creators[0].addEarningStrategy(std::make_shared<BrandDealStrategy>(10000, 1.5, 1.2, 1.1));

    creators.emplace_back("Tushar");
    creators[1].addEarningStrategy(std::make_shared<SubscriptionStrategy>(3200, 2.5));
    creators[1].addEarningStrategy(std::make_shared<LiveGiftStrategy>(500, 4.0));

    creators.emplace_back("Tarun");
    creators[2].addEarningStrategy(std::make_shared<AdRevenueStrategy>(250000, 0.04, 1.5, 0.9, 1.0));
    creators[2].addEarningStrategy(std::make_shared<BrandDealStrategy>(15000, 1.3, 1.0, 0.8));
    creators[2].addEarningStrategy(std::make_shared<LiveGiftStrategy>(150, 8.0));
    creators[2].addEarningStrategy(std::make_shared<SubscriptionStrategy>(8000, 1.5));

    for (const auto& creator : creators)
    {
        std::cout << "--- " << creator.getName() << " ---" << std::endl;
        creator.displayEarnings();
        std::cout << std::endl;
    }

    std::cout << "=== Demo Complete ===" << std::endl;

    return 0;
}
