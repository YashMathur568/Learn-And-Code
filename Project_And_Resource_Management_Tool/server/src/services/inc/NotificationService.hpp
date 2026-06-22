#pragma once

#include "EmailService.hpp"
#include "IResourceRepository.hpp"
#include "ITimesheetRepository.hpp"
#include "IProjectRepository.hpp"
#include "IMilestoneRepository.hpp"
#include "IAllocationRepository.hpp"
#include "LLMProviderManager.hpp"
#include "DatabasePool.hpp"

#include <memory>
#include <string>

class NotificationService {
public:
    NotificationService(
        std::shared_ptr<EmailService>          emailService,
        std::shared_ptr<IResourceRepository>   resourceRepository,
        std::shared_ptr<ITimesheetRepository>  timesheetRepository,
        std::shared_ptr<IProjectRepository>    projectRepository,
        std::shared_ptr<IMilestoneRepository>  milestoneRepository,
        std::shared_ptr<IAllocationRepository> allocationRepository,
        std::shared_ptr<LLMProviderManager>    llmProviderManager
    );


    void processTimesheetReminders();


    void sendAtRiskAlert(int projectId);

private:

    void sendReminder1(int userId, const std::string& weekStart,
                       const std::string& email, const std::string& fullName);
    void sendReminder2(int userId, const std::string& weekStart,
                       const std::string& email, const std::string& fullName);
    void freezeAccount (int userId, const std::string& weekStart,
                        const Resource& resource);


    struct ReminderLog {
        bool     exists{false};
        std::string reminder1SentAt;
        std::string reminder2SentAt;
        std::string frozenAt;
    };
    ReminderLog getLog(int userId, const std::string& weekStart);
    void        upsertLog(int userId, const std::string& weekStart,
                          const std::string& field, const std::string& timesheet);

    static std::string lastMondayString();
    static std::string nowString();
    static bool        isNextDayOrLater(const std::string& isoDatetime);


    std::string buildAtRiskPrompt(const std::string& projectName,
                                  const std::string& milestonesJson,
                                  const std::string& allocationsJson);

    std::shared_ptr<EmailService>          emailService_;
    std::shared_ptr<IResourceRepository>   resourceRepository_;
    std::shared_ptr<ITimesheetRepository>  timesheetRepository_;
    std::shared_ptr<IProjectRepository>    projectRepository_;
    std::shared_ptr<IMilestoneRepository>  milestoneRepository_;
    std::shared_ptr<IAllocationRepository> allocationRepository_;
    std::shared_ptr<LLMProviderManager>    llmProviderManager_;
};
