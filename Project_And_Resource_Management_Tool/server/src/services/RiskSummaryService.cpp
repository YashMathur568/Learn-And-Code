#include "RiskSummaryService.hpp"
#include "../utils/AppException.hpp"

#include <ctime>
#include <iomanip>
#include <sstream>

RiskSummaryService::RiskSummaryService(
    std::shared_ptr<ILLMAdapter>           llmAdapter,
    std::shared_ptr<IProjectRepository>    projectRepository,
    std::shared_ptr<IMilestoneRepository>  milestoneRepository,
    std::shared_ptr<IAllocationRepository> allocationRepository,
    std::shared_ptr<ITimesheetRepository>  timesheetRepository,
    std::shared_ptr<IEmployeeRepository>   employeeRepository
)
    : llmAdapter_(std::move(llmAdapter))
    , projectRepository_(std::move(projectRepository))
    , milestoneRepository_(std::move(milestoneRepository))
    , allocationRepository_(std::move(allocationRepository))
    , timesheetRepository_(std::move(timesheetRepository))
    , employeeRepository_(std::move(employeeRepository)) {}

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
    std::tm nowTm = {};
    localtime_r(&now, &nowTm);
    std::ostringstream dateStream;
    dateStream << std::put_time(&nowTm, "%Y-%m-%d");
    const std::string today = dateStream.str();

    std::ostringstream facts;
    facts << "You are a project risk analyst. Today is " << today << ".\n\n"
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
    for (const auto& alloc : allocations) {
        auto emp = employeeRepository_->findById(alloc.userId);
        const std::string empName = emp.has_value() ? emp->fullName : "Unknown";
        facts << "  - " << empName << " | " << alloc.utilisation << "% | "
              << alloc.fromDate << " to " << alloc.toDate << "\n";

        const auto recentTimesheets = timesheetRepository_->findByUserId(alloc.userId);
        int missedForEmployee = 0;
        for (const auto& ts : recentTimesheets) {
            if (ts.status == "MISSED") {
                ++missedForEmployee;
            }
        }
        if (missedForEmployee > 0) {
            facts << "    WARNING: " << empName << " has " << missedForEmployee << " missed timesheet(s)\n";
            missedCount += missedForEmployee;
        }
    }

    facts << "\nTotal missed timesheets across team: " << missedCount << "\n\n"
          << "Based on the above facts, write a concise risk summary (3-5 sentences) for this project. "
          << "Identify the top risks, mention overdue milestones, team gaps, and missed timesheets. "
          << "Be direct and actionable. Do not use bullet points — write in prose.";

    return facts.str();
}
