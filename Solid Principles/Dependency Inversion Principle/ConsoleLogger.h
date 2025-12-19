#ifndef CONSOLELOGGER_H
#define CONSOLELOGGER_H

#include "Logger.h"

class ConsoleLogger : public Logger
{
public:
    void log(const std::string& message) override;
};

#endif
