#ifndef WORKFLOW_H
#define WORKFLOW_H

#include <string>
#include <vector>
#include <memory>
#include "IWorkflowStep.h"
#include "ILogger.h"

class Workflow
{
private:
    std::string workflowName;
    std::vector<std::shared_ptr<IWorkflowStep>> steps;
    std::shared_ptr<ILogger> logger;

public:
    explicit Workflow(const std::string &workflowName,
                      std::shared_ptr<ILogger> logger = nullptr);

    void addStep(std::shared_ptr<IWorkflowStep> step);
    StepResult run(const std::string &input);
    std::string getWorkflowName() const;
};

#endif
