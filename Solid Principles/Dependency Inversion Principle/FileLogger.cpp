#include <iostream>
#include "FileLogger.h"

void FileLogger::log(const std::string& message)
{
    std::cout << "File Log: " << message << std::endl;
}
