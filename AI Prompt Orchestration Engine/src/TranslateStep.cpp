#include "TranslateStep.h"

TranslateStep::TranslateStep(const std::string &targetLanguage)
    : targetLanguage(targetLanguage) {}

StepResult TranslateStep::execute(const std::string &input)
{
    std::string translatedOutput = "[Translated to " + targetLanguage + "]: " + input;
    return StepResult::success(translatedOutput);
}

std::string TranslateStep::getStepName() const
{
    return "TranslateStep(" + targetLanguage + ")";
}
