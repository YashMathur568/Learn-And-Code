#ifndef ILOGGER_H
#define ILOGGER_H

#include <string>
#include "StepResult.h"

class ILogger
{
public:
    virtual ~ILogger() = default;
    virtual void logStepStart(const std::string &stepName, const std::string &input) = 0;
    virtual void logStepComplete(const std::string &stepName, const StepResult &result) = 0;
    virtual void logStepFailure(const std::string &stepName, const std::string &errorMessage) = 0;
    virtual void logWorkflowStart(const std::string &workflowName) = 0;
    virtual void logWorkflowComplete(const std::string &workflowName, const StepResult &result) = 0;
};

#endif
