#ifndef ELECTRIC_CAR_H
#define ELECTRIC_CAR_H

#include "Vehicle.h"
#include "IRechargeable.h"

class ElectricCar : public Vehicle, public IRechargeable
{
private:
    double batteryLevel;

    static constexpr double MAX_BATTERY_LEVEL = 100.0;
    static constexpr double MIN_BATTERY_LEVEL = 0.0;

public:
    ElectricCar(const std::string& electricCarMake,
                const std::string& electricCarModel,
                int electricCarYear,
                double electricCarPrice,
                double initialBatteryLevel);

    ~ElectricCar() override = default;

    void start() override;
    void displayInfo() const override;

    void charge(double amount) override;
    double getBatteryLevel() const override;
};

#endif
