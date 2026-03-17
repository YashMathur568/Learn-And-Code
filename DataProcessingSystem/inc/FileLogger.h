#ifndef FILE_LOGGER_H
#define FILE_LOGGER_H

#include "ILogger.h"
#include <string>

class FileLogger : public ILogger
{
private:
    std::string filePath;

public:
    FileLogger(const std::string& path);
    void log(const std::string& message) override;
};

#endif
