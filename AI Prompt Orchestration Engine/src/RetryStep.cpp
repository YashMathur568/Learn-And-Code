#include "RetryStep.h"
#include <iostream>

RetryStep::RetryStep(std::shared_ptr<IWorkflowStep> wrappedStep, int maxRetries)
    : wrappedStep(wrappedStep), maxRetries(maxRetries) {}

StepResult RetryStep::execute(const std::string &input)
{
    for (int attempt = 1; attempt <= maxRetries; ++attempt)
    {
        StepResult result = wrappedStep->execute(input);

        if (result.isSuccess())
        {
            return result;
        }

        std::cout << "  [Retry] Attempt " << attempt << "/" << maxRetries
                  << " failed for step '" << wrappedStep->getStepName()
                  << "': " << result.getErrorMessage() << std::endl;
    }

    return StepResult::failure("All " + std::to_string(maxRetries)
                               + " retries exhausted for step '"
                               + wrappedStep->getStepName() + "'");
}

std::string RetryStep::getStepName() const
{
    return "RetryStep(" + wrappedStep->getStepName()
           + ", retries=" + std::to_string(maxRetries) + ")";
}
