#include "GenerateStep.h"

GenerateStep::GenerateStep(const std::string &modelName)
    : modelName(modelName) {}

StepResult GenerateStep::execute(const std::string &input)
{
    std::string generatedOutput = "[Generated using " + modelName + "]: " + input;
    return StepResult::success(generatedOutput);
}

std::string GenerateStep::getStepName() const
{
    return "GenerateStep(" + modelName + ")";
}
