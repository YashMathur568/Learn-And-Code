#ifndef ENRICH_CONTEXT_STEP_H
#define ENRICH_CONTEXT_STEP_H

#include "IWorkflowStep.h"

class EnrichContextStep : public IWorkflowStep
{
private:
    std::string contextSource;

public:
    explicit EnrichContextStep(const std::string &contextSource = "knowledge-base");
    StepResult execute(const std::string &input) override;
    std::string getStepName() const override;
};

#endif
