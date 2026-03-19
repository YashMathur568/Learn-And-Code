#ifndef MOTORCYCLE_H
#define MOTORCYCLE_H

#include "Vehicle.h"
#include "IRefuelable.h"

class Motorcycle : public Vehicle, public IRefuelable
{
private:
    double fuelLevel;
    bool hasSidecar;

    static constexpr double MAX_FUEL_LEVEL = 100.0;
    static constexpr double MIN_FUEL_LEVEL = 0.0;

public:
    Motorcycle(const std::string& motorcycleMake,
               const std::string& motorcycleModel,
               int motorcycleYear,
               double motorcyclePrice,
               double initialFuelLevel,
               bool sidecarEquipped);

    ~Motorcycle() override = default;

    void start() override;
    void displayInfo() const override;

    void refuel(double amount) override;
    double getFuelLevel() const override;

    bool getHasSidecar() const;
};

#endif
