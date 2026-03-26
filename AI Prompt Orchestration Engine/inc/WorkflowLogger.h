#ifndef WORKFLOW_LOGGER_H
#define WORKFLOW_LOGGER_H

#include "ILogger.h"

class WorkflowLogger : public ILogger
{
public:
    void logStepStart(const std::string &stepName, const std::string &input) override;
    void logStepComplete(const std::string &stepName, const StepResult &result) override;
    void logStepFailure(const std::string &stepName, const std::string &errorMessage) override;
    void logWorkflowStart(const std::string &workflowName) override;
    void logWorkflowComplete(const std::string &workflowName, const StepResult &result) override;
};

#endif
