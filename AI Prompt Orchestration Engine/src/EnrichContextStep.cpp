#include "EnrichContextStep.h"

EnrichContextStep::EnrichContextStep(const std::string &contextSource)
    : contextSource(contextSource) {}

StepResult EnrichContextStep::execute(const std::string &input)
{
    std::string enrichedOutput = "[Enriched from " + contextSource + "]: " + input;
    return StepResult::success(enrichedOutput);
}

std::string EnrichContextStep::getStepName() const
{
    return "EnrichContextStep(" + contextSource + ")";
}
