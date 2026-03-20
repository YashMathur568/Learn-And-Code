#include "Car.h"
#include "Motorcycle.h"
#include "ElectricCar.h"
#include "VehicleManager.h"
#include <memory>
#include <iostream>

int main()
{
    std::cout << "=== Vehicle Management Demo ===" << std::endl << std::endl;

    auto hondaAccord = std::make_shared<Car>("Honda", "Accord", 2023, 28000, 100);
    auto harleyStreet = std::make_shared<Motorcycle>("Harley-Davidson", "Street 750", 2022, 7500, 80, false);
    auto teslaModel3 = std::make_shared<ElectricCar>("Tesla", "Model 3", 2023, 42000, 100);

    std::cout << "Testing Vehicles:" << std::endl;
    hondaAccord->start();
    hondaAccord->displayInfo();
    hondaAccord->stop();

    std::cout << std::endl;
    harleyStreet->start();
    harleyStreet->displayInfo();

    std::cout << std::endl;
    teslaModel3->start();
    teslaModel3->displayInfo();

    VehicleManager manager;
    manager.addVehicle(hondaAccord);
    manager.addVehicle(harleyStreet);
    manager.addVehicle(teslaModel3);

    manager.displayAll();
    std::cout << "\nTotal Value: $" << manager.calculateTotalValue() << std::endl;

    std::cout << "\nStarting all vehicles:" << std::endl;
    manager.startAllVehicles();

    std::cout << "\n=== Encapsulation Demo ===" << std::endl;
    std::cout << "Attempting to set negative price:" << std::endl;
    hondaAccord->setPrice(-1000);
    std::cout << "Car price after invalid set: $" << hondaAccord->getPrice() << std::endl;

    std::cout << "Attempting to set fuel level beyond 100%:" << std::endl;
    hondaAccord->refuel(500);
    std::cout << "Car fuel after capped refuel: " << hondaAccord->getFuelLevel() << "%" << std::endl;

    std::cout << "\n=== Demo Complete ===" << std::endl;

    return 0;
}
