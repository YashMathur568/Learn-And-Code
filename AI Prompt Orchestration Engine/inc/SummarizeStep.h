#ifndef SUMMARIZE_STEP_H
#define SUMMARIZE_STEP_H

#include "IWorkflowStep.h"

class SummarizeStep : public IWorkflowStep
{
private:
    int maxLength;

public:
    explicit SummarizeStep(int maxLength = 200);
    StepResult execute(const std::string &input) override;
    std::string getStepName() const override;
};

#endif
