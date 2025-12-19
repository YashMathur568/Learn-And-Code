#include "Application.h"

Application::Application(Logger& logger)
    : logger(logger)
{
}

void Application::run()
{
    logger.log("Application started");
}
