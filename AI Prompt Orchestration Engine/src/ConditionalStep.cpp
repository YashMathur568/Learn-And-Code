#include "ConditionalStep.h"
#include <iostream>

ConditionalStep::ConditionalStep(std::shared_ptr<IWorkflowStep> wrappedStep,
                                 std::function<bool(const std::string &)> condition)
    : wrappedStep(wrappedStep), condition(condition) {}

StepResult ConditionalStep::execute(const std::string &input)
{
    if (!condition(input))
    {
        std::cout << "  [Conditional] Skipping step '"
                  << wrappedStep->getStepName()
                  << "' - condition not met." << std::endl;
        return StepResult::success(input);
    }

    return wrappedStep->execute(input);
}

std::string ConditionalStep::getStepName() const
{
    return "ConditionalStep(" + wrappedStep->getStepName() + ")";
}
