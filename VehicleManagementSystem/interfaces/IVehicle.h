#ifndef IVEHICLE_H
#define IVEHICLE_H

#include <string>

class IVehicle
{
public:
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void displayInfo() const = 0;
    virtual double getPrice() const = 0;
    virtual std::string getMake() const = 0;
    virtual std::string getModel() const = 0;
    virtual ~IVehicle() = default;
};

#endif
