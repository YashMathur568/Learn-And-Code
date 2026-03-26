#ifndef RETRY_STEP_H
#define RETRY_STEP_H

#include <memory>
#include "IWorkflowStep.h"

class RetryStep : public IWorkflowStep
{
private:
    std::shared_ptr<IWorkflowStep> wrappedStep;
    int maxRetries;

public:
    RetryStep(std::shared_ptr<IWorkflowStep> wrappedStep, int maxRetries = 3);
    StepResult execute(const std::string &input) override;
    std::string getStepName() const override;
};

#endif
