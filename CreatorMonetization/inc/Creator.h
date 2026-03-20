#ifndef CREATOR_H
#define CREATOR_H

#include "IEarningStrategy.h"
#include <string>
#include <vector>
#include <memory>

class Creator
{
private:
    std::string name;
    std::vector<std::shared_ptr<IEarningStrategy>> earningStrategies;

public:
    Creator(const std::string& creatorName);

    std::string getName() const;
    void addEarningStrategy(const std::shared_ptr<IEarningStrategy>& strategy);
    double calculateTotalEarnings() const;
    void displayEarnings() const;
};

#endif
