#include "WorkflowLogger.h"
#include <iostream>

void WorkflowLogger::logStepStart(const std::string &stepName, const std::string &input)
{
    std::cout << "  [LOG] Step START: " << stepName << std::endl;
    std::cout << "        Input: " << input.substr(0, 80)
              << (input.length() > 80 ? "..." : "") << std::endl;
}

void WorkflowLogger::logStepComplete(const std::string &stepName, const StepResult &result)
{
    std::string output = result.getOutput();
    std::cout << "  [LOG] Step COMPLETE: " << stepName << std::endl;
    std::cout << "        Output: " << output.substr(0, 80)
              << (output.length() > 80 ? "..." : "") << std::endl;
}

void WorkflowLogger::logStepFailure(const std::string &stepName, const std::string &errorMessage)
{
    std::cout << "  [LOG] Step FAILED: " << stepName << std::endl;
    std::cout << "        Error: " << errorMessage << std::endl;
}

void WorkflowLogger::logWorkflowStart(const std::string &workflowName)
{
    std::cout << "\n========================================" << std::endl;
    std::cout << "[LOG] Workflow START: " << workflowName << std::endl;
    std::cout << "========================================" << std::endl;
}

void WorkflowLogger::logWorkflowComplete(const std::string &workflowName, const StepResult &result)
{
    std::cout << "========================================" << std::endl;
    if (result.isSuccess())
    {
        std::cout << "[LOG] Workflow COMPLETE: " << workflowName << " (SUCCESS)" << std::endl;
    }
    else
    {
        std::cout << "[LOG] Workflow COMPLETE: " << workflowName << " (FAILED)" << std::endl;
        std::cout << "      Error: " << result.getErrorMessage() << std::endl;
    }
    std::cout << "========================================\n" << std::endl;
}
