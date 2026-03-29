#ifndef APPLICATION_H
#define APPLICATION_H

#include "Logger.h"

class Application
{
public:
    explicit Application(Logger& logger);

    void run();

private:
    Logger& logger;
};

#endif
