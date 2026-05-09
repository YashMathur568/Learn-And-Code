#pragma once

#include <string>

class IInputHandler {
public:
    virtual ~IInputHandler() = default;
    virtual std::string readString(const std::string& prompt) = 0;
    virtual double readDouble(const std::string& prompt) = 0;
    virtual int readInt(const std::string& prompt) = 0;
    virtual std::string readDate(const std::string& prompt) = 0;
};
