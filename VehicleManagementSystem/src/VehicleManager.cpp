#include "VehicleManager.h"

void VehicleManager::addVehicle(const std::shared_ptr<IVehicle>& vehicle)
{
    vehicles.push_back(vehicle);
    std::cout << vehicle->getMake() << " " << vehicle->getModel() << " added." << std::endl;
}

void VehicleManager::displayAll() const
{
    std::cout << "\n=== Vehicles ===" << std::endl;
    for (const auto& vehicle : vehicles)
    {
        vehicle->displayInfo();
    }
}

double VehicleManager::calculateTotalValue() const
{
    double totalValue = 0.0;
    for (const auto& vehicle : vehicles)
    {
        totalValue += vehicle->getPrice();
    }
    return totalValue;
}

void VehicleManager::startAllVehicles()
{
    for (const auto& vehicle : vehicles)
    {
        vehicle->start();
    }
}
