#ifndef TRANSLATE_STEP_H
#define TRANSLATE_STEP_H

#include "IWorkflowStep.h"

class TranslateStep : public IWorkflowStep
{
private:
    std::string targetLanguage;

public:
    explicit TranslateStep(const std::string &targetLanguage = "Spanish");
    StepResult execute(const std::string &input) override;
    std::string getStepName() const override;
};

#endif
