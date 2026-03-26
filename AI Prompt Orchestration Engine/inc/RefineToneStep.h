#ifndef REFINE_TONE_STEP_H
#define REFINE_TONE_STEP_H

#include "IWorkflowStep.h"

class RefineToneStep : public IWorkflowStep
{
private:
    std::string targetTone;

public:
    explicit RefineToneStep(const std::string &targetTone = "professional");
    StepResult execute(const std::string &input) override;
    std::string getStepName() const override;
};

#endif
