#ifndef STEP_RESULT_H
#define STEP_RESULT_H

#include <string>

class StepResult
{
private:
    bool successFlag;
    std::string output;
    std::string errorMessage;

public:
    StepResult(bool successFlag, const std::string &output, const std::string &errorMessage = "");

    bool isSuccess() const;
    std::string getOutput() const;
    std::string getErrorMessage() const;

    static StepResult success(const std::string &output);
    static StepResult failure(const std::string &errorMessage);
};

#endif
