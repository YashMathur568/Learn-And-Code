#ifndef IATM_DEVICE_H
#define IATM_DEVICE_H

#include <string>

class IATMDevice
{
public:
    virtual void withdraw(const std::string& accountId, double amount) = 0;
    virtual ~IATMDevice() = default;
};

#endif
