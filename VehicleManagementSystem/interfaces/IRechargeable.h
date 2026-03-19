#ifndef IRECHARGEABLE_H
#define IRECHARGEABLE_H

class IRechargeable
{
public:
    virtual void charge(double amount) = 0;
    virtual double getBatteryLevel() const = 0;
    virtual ~IRechargeable() = default;
};

#endif
