#ifndef CAR_H
#define CAR_H

#include "Vehicle.h"
#include "IRefuelable.h"

class Car : public Vehicle, public IRefuelable
{
private:
    double fuelLevel;

    static constexpr double MAX_FUEL_LEVEL = 100.0;
    static constexpr double MIN_FUEL_LEVEL = 0.0;

public:
    Car(const std::string& carMake,
        const std::string& carModel,
        int carYear,
        double carPrice,
        double initialFuelLevel);

    ~Car() override = default;

    void start() override;
    void displayInfo() const override;

    void refuel(double amount) override;
    double getFuelLevel() const override;
};

#endif
