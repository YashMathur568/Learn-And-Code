#include <iostream>
#include "ConsoleLogger.h"

void ConsoleLogger::log(const std::string& message)
{
    std::cout << "Console Log: " << message << std::endl;
}
