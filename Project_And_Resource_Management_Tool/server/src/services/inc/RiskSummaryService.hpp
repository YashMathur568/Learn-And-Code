#pragma once

#include "IRiskSummaryService.hpp"
#include "ILLMAdapter.hpp"
#include "IProjectRepository.hpp"
#include "IMilestoneRepository.hpp"
#include "IAllocationRepository.hpp"
#include "ITimesheetRepository.hpp"
#include "IResourceRepository.hpp"

#include <memory>
#include <string>

class RiskSummaryService : public IRiskSummaryService {
public:
    RiskSummaryService(
        std::shared_ptr<ILLMAdapter>           llmAdapter,
        std::shared_ptr<IProjectRepository>    projectRepository,
        std::shared_ptr<IMilestoneRepository>  milestoneRepository,
        std::shared_ptr<IAllocationRepository> allocationRepository,
        std::shared_ptr<ITimesheetRepository>  timesheetRepository,
        std::shared_ptr<IResourceRepository>   resourceRepository
    );

    std::string generateSummary(int projectId, int managerUserId) override;

private:
    std::string buildFactsPrompt(int projectId) const;

    std::shared_ptr<ILLMAdapter>           llmAdapter_;
    std::shared_ptr<IProjectRepository>    projectRepository_;
    std::shared_ptr<IMilestoneRepository>  milestoneRepository_;
    std::shared_ptr<IAllocationRepository> allocationRepository_;
    std::shared_ptr<ITimesheetRepository>  timesheetRepository_;
    std::shared_ptr<IResourceRepository>   resourceRepository_;
};
