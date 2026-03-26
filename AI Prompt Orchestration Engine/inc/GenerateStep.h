#ifndef GENERATE_STEP_H
#define GENERATE_STEP_H

#include "IWorkflowStep.h"

class GenerateStep : public IWorkflowStep
{
private:
    std::string modelName;

public:
    explicit GenerateStep(const std::string &modelName = "default-llm");
    StepResult execute(const std::string &input) override;
    std::string getStepName() const override;
};

#endif
