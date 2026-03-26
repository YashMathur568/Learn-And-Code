#ifndef ANALYZE_STEP_H
#define ANALYZE_STEP_H

#include "IWorkflowStep.h"

class AnalyzeStep : public IWorkflowStep
{
private:
    std::string analysisType;

public:
    explicit AnalyzeStep(const std::string &analysisType = "general");
    StepResult execute(const std::string &input) override;
    std::string getStepName() const override;
};

#endif
