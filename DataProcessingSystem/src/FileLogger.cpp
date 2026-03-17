#include "FileLogger.h"
#include <fstream>
#include <ctime>

FileLogger::FileLogger(const std::string& path)
    : filePath(path)
{
}

void FileLogger::log(const std::string& message)
{
    std::ofstream file(filePath, std::ios::app);

    std::time_t now = std::time(nullptr);
    file << std::ctime(&now) << ": " << message << "\n";
}
