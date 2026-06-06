#pragma once

#include "../ai/ILLMAdapter.hpp"
#include "../repositories/IProjectRepository.hpp"
#include "../repositories/IMilestoneRepository.hpp"
#include "../repositories/IAllocationRepository.hpp"
#include "../repositories/ITimesheetRepository.hpp"
#include "../repositories/IEmployeeRepository.hpp"

#include <memory>
#include <string>

class RiskSummaryService {
public:
    RiskSummaryService(
        std::shared_ptr<ILLMAdapter>           llmAdapter,
        std::shared_ptr<IProjectRepository>    projectRepository,
        std::shared_ptr<IMilestoneRepository>  milestoneRepository,
        std::shared_ptr<IAllocationRepository> allocationRepository,
        std::shared_ptr<ITimesheetRepository>  timesheetRepository,
        std::shared_ptr<IEmployeeRepository>   employeeRepository
    );

    std::string generateSummary(int projectId, int managerEmployeeId);

private:
    std::string buildFactsPrompt(int projectId) const;

    std::shared_ptr<ILLMAdapter>           llmAdapter_;
    std::shared_ptr<IProjectRepository>    projectRepository_;
    std::shared_ptr<IMilestoneRepository>  milestoneRepository_;
    std::shared_ptr<IAllocationRepository> allocationRepository_;
    std::shared_ptr<ITimesheetRepository>  timesheetRepository_;
    std::shared_ptr<IEmployeeRepository>   employeeRepository_;
};
