#include "FallbackStep.h"
#include <iostream>

FallbackStep::FallbackStep(std::shared_ptr<IWorkflowStep> primaryStep,
                           std::shared_ptr<IWorkflowStep> fallbackStep)
    : primaryStep(primaryStep), fallbackStep(fallbackStep) {}

StepResult FallbackStep::execute(const std::string &input)
{
    StepResult primaryResult = primaryStep->execute(input);

    if (primaryResult.isSuccess())
    {
        return primaryResult;
    }

    std::cout << "  [Fallback] Primary step '" << primaryStep->getStepName()
              << "' failed. Switching to fallback: '"
              << fallbackStep->getStepName() << "'" << std::endl;

    return fallbackStep->execute(input);
}

std::string FallbackStep::getStepName() const
{
    return "FallbackStep(" + primaryStep->getStepName()
           + " -> " + fallbackStep->getStepName() + ")";
}
