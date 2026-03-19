#include "Motorcycle.h"

Motorcycle::Motorcycle(const std::string& motorcycleMake,
                       const std::string& motorcycleModel,
                       int motorcycleYear,
                       double motorcyclePrice,
                       double initialFuelLevel,
                       bool sidecarEquipped)
    : Vehicle(motorcycleMake, motorcycleModel, motorcycleYear, motorcyclePrice),
      fuelLevel(MIN_FUEL_LEVEL),
      hasSidecar(sidecarEquipped)
{
    if (initialFuelLevel >= MIN_FUEL_LEVEL && initialFuelLevel <= MAX_FUEL_LEVEL)
    {
        fuelLevel = initialFuelLevel;
    }
    else
    {
        std::cout << "Warning: Invalid fuel level. Defaulting to 0%." << std::endl;
    }
}

void Motorcycle::start()
{
    if (fuelLevel > MIN_FUEL_LEVEL)
    {
        setRunning(true);
        std::cout << getMake() << " " << getModel() << " started." << std::endl;
    }
    else
    {
        std::cout << "Cannot start - no fuel!" << std::endl;
    }
}

void Motorcycle::displayInfo() const
{
    std::cout << "Motorcycle: " << getYearOfManufacture() << " "
              << getMake() << " " << getModel()
              << ", Sidecar: " << (hasSidecar ? "Yes" : "No")
              << ", Price: $" << getPrice() << std::endl;
}

void Motorcycle::refuel(double amount)
{
    if (amount <= 0)
    {
        std::cout << "Error: Refuel amount must be positive." << std::endl;
        return;
    }
    fuelLevel += amount;
    if (fuelLevel > MAX_FUEL_LEVEL)
    {
        fuelLevel = MAX_FUEL_LEVEL;
    }
    std::cout << "Refueled. Fuel level: " << fuelLevel << "%" << std::endl;
}

double Motorcycle::getFuelLevel() const
{
    return fuelLevel;
}

bool Motorcycle::getHasSidecar() const
{
    return hasSidecar;
}
