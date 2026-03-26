#ifndef FALLBACK_STEP_H
#define FALLBACK_STEP_H

#include <memory>
#include "IWorkflowStep.h"

class FallbackStep : public IWorkflowStep
{
private:
    std::shared_ptr<IWorkflowStep> primaryStep;
    std::shared_ptr<IWorkflowStep> fallbackStep;

public:
    FallbackStep(std::shared_ptr<IWorkflowStep> primaryStep,
                 std::shared_ptr<IWorkflowStep> fallbackStep);
    StepResult execute(const std::string &input) override;
    std::string getStepName() const override;
};

#endif
