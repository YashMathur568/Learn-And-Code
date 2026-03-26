#include "RefineToneStep.h"

RefineToneStep::RefineToneStep(const std::string &targetTone)
    : targetTone(targetTone) {}

StepResult RefineToneStep::execute(const std::string &input)
{
    std::string refinedOutput = "[Refined to " + targetTone + " tone]: " + input;
    return StepResult::success(refinedOutput);
}

std::string RefineToneStep::getStepName() const
{
    return "RefineToneStep(" + targetTone + ")";
}
