#ifndef FILELOGGER_H
#define FILELOGGER_H

#include "Logger.h"

class FileLogger : public Logger
{
public:
    void log(const std::string& message) override;
};

#endif
