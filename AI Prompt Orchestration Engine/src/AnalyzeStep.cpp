#include "AnalyzeStep.h"

AnalyzeStep::AnalyzeStep(const std::string &analysisType)
    : analysisType(analysisType) {}

StepResult AnalyzeStep::execute(const std::string &input)
{
    std::string analyzedOutput = "[Analyzed (" + analysisType + ")]: " + input;
    return StepResult::success(analyzedOutput);
}

std::string AnalyzeStep::getStepName() const
{
    return "AnalyzeStep(" + analysisType + ")";
}
