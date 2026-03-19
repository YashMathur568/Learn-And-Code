#include "ElectricCar.h"

ElectricCar::ElectricCar(const std::string& electricCarMake,
                         const std::string& electricCarModel,
                         int electricCarYear,
                         double electricCarPrice,
                         double initialBatteryLevel)
    : Vehicle(electricCarMake, electricCarModel, electricCarYear, electricCarPrice),
      batteryLevel(MIN_BATTERY_LEVEL)
{
    if (initialBatteryLevel >= MIN_BATTERY_LEVEL && initialBatteryLevel <= MAX_BATTERY_LEVEL)
    {
        batteryLevel = initialBatteryLevel;
    }
    else
    {
        std::cout << "Warning: Invalid battery level. Defaulting to 0%." << std::endl;
    }
}

void ElectricCar::start()
{
    if (batteryLevel > MIN_BATTERY_LEVEL)
    {
        setRunning(true);
        std::cout << getMake() << " " << getModel() << " electric motor started." << std::endl;
    }
    else
    {
        std::cout << "Cannot start - battery dead!" << std::endl;
    }
}

void ElectricCar::displayInfo() const
{
    std::cout << "Electric Car: " << getYearOfManufacture() << " "
              << getMake() << " " << getModel()
              << ", Price: $" << getPrice() << std::endl;
}

void ElectricCar::charge(double amount)
{
    if (amount <= 0)
    {
        std::cout << "Error: Charge amount must be positive." << std::endl;
        return;
    }
    batteryLevel += amount;
    if (batteryLevel > MAX_BATTERY_LEVEL)
    {
        batteryLevel = MAX_BATTERY_LEVEL;
    }
    std::cout << "Charged. Battery level: " << batteryLevel << "%" << std::endl;
}

double ElectricCar::getBatteryLevel() const
{
    return batteryLevel;
}
