#ifndef IWORKFLOW_STEP_H
#define IWORKFLOW_STEP_H

#include <string>
#include "StepResult.h"

class IWorkflowStep
{
public:
    virtual ~IWorkflowStep() = default;
    virtual StepResult execute(const std::string &input) = 0;
    virtual std::string getStepName() const = 0;
};

#endif
