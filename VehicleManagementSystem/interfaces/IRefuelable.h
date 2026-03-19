#ifndef IREFUELABLE_H
#define IREFUELABLE_H

class IRefuelable
{
public:
    virtual void refuel(double amount) = 0;
    virtual double getFuelLevel() const = 0;
    virtual ~IRefuelable() = default;
};

#endif
