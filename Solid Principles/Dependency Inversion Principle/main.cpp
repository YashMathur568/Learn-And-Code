#include "Application.h"
#include "ConsoleLogger.h"
#include "FileLogger.h"

// Application does not depend on ConsoleLogger or FileLogger
// Both depend on Logger abstraction
// Easy to switch logging mechanism
// Easy to unit test (mock Logger)

int main()
{
    ConsoleLogger consoleLogger;
    Application appWithConsoleLogger(consoleLogger);
    appWithConsoleLogger.run();

    FileLogger fileLogger;
    Application appWithFileLogger(fileLogger);
    appWithFileLogger.run();

    return 0;
}
