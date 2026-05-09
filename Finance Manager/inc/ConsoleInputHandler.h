#pragma once

#include "IInputHandler.h"

class ConsoleInputHandler : public IInputHandler {
public:
    std::string readString(const std::string& prompt) override;
    double readDouble(const std::string& prompt) override;
    int readInt(const std::string& prompt) override;
    std::string readDate(const std::string& prompt) override;
};
