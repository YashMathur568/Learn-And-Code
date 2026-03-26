#ifndef CONDITIONAL_STEP_H
#define CONDITIONAL_STEP_H

#include <memory>
#include <functional>
#include "IWorkflowStep.h"

class ConditionalStep : public IWorkflowStep
{
private:
    std::shared_ptr<IWorkflowStep> wrappedStep;
    std::function<bool(const std::string &)> condition;

public:
    ConditionalStep(std::shared_ptr<IWorkflowStep> wrappedStep,
                    std::function<bool(const std::string &)> condition);
    StepResult execute(const std::string &input) override;
    std::string getStepName() const override;
};

#endif
