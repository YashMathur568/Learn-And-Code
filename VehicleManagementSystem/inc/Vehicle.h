#ifndef VEHICLE_H
#define VEHICLE_H

#include "IVehicle.h"
#include <string>
#include <iostream>

class Vehicle : public IVehicle
{
private:
    std::string make;
    std::string model;
    int yearOfManufacture;
    double price;
    bool running;

    static constexpr double MAX_PRICE = 1000000.0;
    static constexpr double MIN_PRICE = 0.0;

public:
    Vehicle(const std::string& vehicleMake,
            const std::string& vehicleModel,
            int vehicleYear,
            double vehiclePrice);

    virtual ~Vehicle() = default;

    std::string getMake() const override;
    std::string getModel() const override;
    int getYearOfManufacture() const;
    double getPrice() const override;
    bool isRunning() const;

    void setPrice(double newPrice);

    void stop() override;

protected:
    void setRunning(bool state);
};

#endif
