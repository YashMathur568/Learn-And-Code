#include "StepResult.h"

StepResult::StepResult(bool successFlag, const std::string &output, const std::string &errorMessage)
    : successFlag(successFlag), output(output), errorMessage(errorMessage) {}

bool StepResult::isSuccess() const
{
    return successFlag;
}

std::string StepResult::getOutput() const
{
    return output;
}

std::string StepResult::getErrorMessage() const
{
    return errorMessage;
}

StepResult StepResult::success(const std::string &output)
{
    return StepResult(true, output, "");
}

StepResult StepResult::failure(const std::string &errorMessage)
{
    return StepResult(false, "", errorMessage);
}
