#include "Car.h"

Car::Car(const std::string& carMake,
         const std::string& carModel,
         int carYear,
         double carPrice,
         double initialFuelLevel)
    : Vehicle(carMake, carModel, carYear, carPrice),
      fuelLevel(MIN_FUEL_LEVEL)
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

void Car::start()
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

void Car::displayInfo() const
{
    std::cout << "Car: " << getYearOfManufacture() << " "
              << getMake() << " " << getModel()
              << ", Price: $" << getPrice() << std::endl;
}

void Car::refuel(double amount)
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

double Car::getFuelLevel() const
{
    return fuelLevel;
}
