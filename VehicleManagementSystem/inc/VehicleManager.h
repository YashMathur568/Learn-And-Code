#ifndef VEHICLE_MANAGER_H
#define VEHICLE_MANAGER_H

#include "IVehicle.h"
#include <vector>
#include <memory>
#include <iostream>

class VehicleManager
{
private:
    std::vector<std::shared_ptr<IVehicle>> vehicles;

public:
    void addVehicle(const std::shared_ptr<IVehicle>& vehicle);
    void displayAll() const;
    double calculateTotalValue() const;
    void startAllVehicles();
};

#endif
