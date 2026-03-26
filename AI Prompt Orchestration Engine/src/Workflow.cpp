#include "Workflow.h"
#include <iostream>

Workflow::Workflow(const std::string &workflowName, std::shared_ptr<ILogger> logger)
    : workflowName(workflowName), logger(logger) {}

void Workflow::addStep(std::shared_ptr<IWorkflowStep> step)
{
    steps.push_back(step);
}

StepResult Workflow::run(const std::string &input)
{
    if (logger)
    {
        logger->logWorkflowStart(workflowName);
    }

    std::string currentOutput = input;

    for (size_t stepIndex = 0; stepIndex < steps.size(); ++stepIndex)
    {
        const auto &currentStep = steps[stepIndex];

        if (logger)
        {
            logger->logStepStart(currentStep->getStepName(), currentOutput);
        }

        StepResult result = currentStep->execute(currentOutput);

        if (!result.isSuccess())
        {
            if (logger)
            {
                logger->logStepFailure(currentStep->getStepName(), result.getErrorMessage());
            }

            StepResult failureResult = StepResult::failure(
                "Workflow '" + workflowName + "' failed at step "
                + std::to_string(stepIndex + 1) + " ("
                + currentStep->getStepName() + "): "
                + result.getErrorMessage());

            if (logger)
            {
                logger->logWorkflowComplete(workflowName, failureResult);
            }

            return failureResult;
        }

        if (logger)
        {
            logger->logStepComplete(currentStep->getStepName(), result);
        }

        currentOutput = result.getOutput();
    }

    StepResult finalResult = StepResult::success(currentOutput);

    if (logger)
    {
        logger->logWorkflowComplete(workflowName, finalResult);
    }

    return finalResult;
}

std::string Workflow::getWorkflowName() const
{
    return workflowName;
}
