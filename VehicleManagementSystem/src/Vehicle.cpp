#include "Vehicle.h"

Vehicle::Vehicle(const std::string& vehicleMake,
                 const std::string& vehicleModel,
                 int vehicleYear,
                 double vehiclePrice)
    : make(vehicleMake),
      model(vehicleModel),
      yearOfManufacture(vehicleYear),
      price(MIN_PRICE),
      running(false)
{
    setPrice(vehiclePrice);
}

std::string Vehicle::getMake() const
{
    return make;
}

std::string Vehicle::getModel() const
{
    return model;
}

int Vehicle::getYearOfManufacture() const
{
    return yearOfManufacture;
}

double Vehicle::getPrice() const
{
    return price;
}

bool Vehicle::isRunning() const
{
    return running;
}

void Vehicle::setPrice(double newPrice)
{
    if (newPrice < MIN_PRICE)
    {
        std::cout << "Error: Price cannot be negative. Price not updated." << std::endl;
        return;
    }
    if (newPrice > MAX_PRICE)
    {
        std::cout << "Error: Price cannot exceed $" << MAX_PRICE << ". Price not updated." << std::endl;
        return;
    }
    price = newPrice;
}

void Vehicle::stop()
{
    running = false;
    std::cout << make << " " << model << " stopped." << std::endl;
}

void Vehicle::setRunning(bool state)
{
    running = state;
}
