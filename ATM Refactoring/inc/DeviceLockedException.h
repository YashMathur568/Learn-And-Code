#ifndef DEVICE_LOCKED_EXCEPTION_H
#define DEVICE_LOCKED_EXCEPTION_H

#include <stdexcept>
#include <string>

class DeviceLockedException : public std::runtime_error
{
public:
    explicit DeviceLockedException(const std::string& message)
        : std::runtime_error(message) {}
};

#endif
