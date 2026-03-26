#include "SummarizeStep.h"

SummarizeStep::SummarizeStep(int maxLength)
    : maxLength(maxLength) {}

StepResult SummarizeStep::execute(const std::string &input)
{
    std::string summarizedOutput = "[Summary (max " + std::to_string(maxLength) + " chars)]: " + input;
    return StepResult::success(summarizedOutput);
}

std::string SummarizeStep::getStepName() const
{
    return "SummarizeStep(max=" + std::to_string(maxLength) + ")";
}
