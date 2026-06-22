#include "RiskSummaryService.hpp"
#include "AppException.hpp"

#include <ctime>
#include <iomanip>
#include <sstream>

RiskSummaryService::RiskSummaryService(
    std::shared_ptr<ILLMAdapter>           llmAdapter,
    std::shared_ptr<IProjectRepository>    projectRepository,
    std::shared_ptr<IMilestoneRepository>  milestoneRepository,
    std::shared_ptr<IAllocationRepository> allocationRepository,
    std::shared_ptr<ITimesheetRepository>  timesheetRepository,
    std::shared_ptr<IResourceRepository>   resourceRepository
)
    : llmAdapter_(std::move(llmAdapter))
    , projectRepository_(std::move(projectRepository))
    , milestoneRepository_(std::move(milestoneRepository))
    , allocationRepository_(std::move(allocationRepository))
    , timesheetRepository_(std::move(timesheetRepository))
    , resourceRepository_(std::move(resourceRepository)) {}

std::string RiskSummaryService::generateSummary(int projectId, int managerUserId) {
    auto optProject = projectRepository_->findById(projectId);
    if (!optProject.has_value()) {
        throw NotFoundException("Project with ID " + std::to_string(projectId) + " not found.");
    }
    if (optProject->managerId != managerUserId) {
        throw UnauthorizedException("You do not manage this project.");
    }

    const std::string prompt = buildFactsPrompt(projectId);
    return llmAdapter_->generate(prompt);
}

std::string RiskSummaryService::buildFactsPrompt(int projectId) const {
    const auto project    = projectRepository_->findById(projectId).value();
    const auto milestones = milestoneRepository_->findByProjectId(projectId);
    const auto allocations = allocationRepository_->findActiveByProjectId(projectId);

    const std::time_t now = std::time(nullptr);
    std::tm nowTimeStructure = {};
    localtime_r(&now, &nowTimeStructure);
    std::ostringstream dateStream;
    dateStream << std::put_time(&nowTimeStructure, "%Y-%m-%d");
    const std::string today = dateStream.str();

    std::ostringstream facts;
    facts << "You are allocation project risk analyst. Today is " << today << ".\n\n"
          << "Project: " << project.name << "\n"
          << "Status: "  << project.status << " | Health: " << project.health << "\n"
          << "Timeline: " << project.startDate << " to " << project.endDate << "\n\n";

    facts << "Milestones (" << milestones.size() << "):\n";
    for (const auto& milestone : milestones) {
        facts << "  - " << milestone.title
              << " | Due: " << milestone.dueDate
              << " | Status: " << milestone.status;
        if (milestone.status != "DONE" && milestone.dueDate < today) {
            facts << " [OVERDUE]";
        }
        facts << "\n";
    }

    facts << "\nActive Team (" << allocations.size() << " allocations):\n";
    int missedCount = 0;
    for (const auto& allocation : allocations) {
        auto resource = resourceRepository_->findById(allocation.userId);
        const std::string resName = resource.has_value() ? resource->fullName : "Unknown";
        facts << "  - " << resName << " | " << allocation.utilisation << "% | "
              << allocation.fromDate << " to " << allocation.toDate << "\n";

        const auto recentTimesheets = timesheetRepository_->findByUserId(allocation.userId);
        int missedForResource = 0;
        for (const auto& timesheet : recentTimesheets) {
            if (timesheet.status == "MISSED") {
                ++missedForResource;
            }
        }
        if (missedForResource > 0) {
            facts << "    WARNING: " << resName << " has " << missedForResource << " missed timesheet(s)\n";
            missedCount += missedForResource;
        }
    }

    facts << "\nTotal missed timesheets across team: " << missedCount << "\n\n"
          << "Based on the above facts, write allocation concise risk summary (3-5 sentences) for this project. "
          << "Identify the top risks, mention overdue milestones, team gaps, and missed timesheets. "
          << "Be direct and actionable. Do not use bullet points — write in prose.";

    return facts.str();
}
