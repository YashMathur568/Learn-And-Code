#pragma once

#include "../email/EmailService.hpp"
#include "../repositories/IEmployeeRepository.hpp"
#include "../repositories/ITimesheetRepository.hpp"
#include "../repositories/IProjectRepository.hpp"
#include "../repositories/IMilestoneRepository.hpp"
#include "../repositories/IAllocationRepository.hpp"
#include "../ai/ILLMAdapter.hpp"
#include "../utils/DatabasePool.hpp"

#include <memory>
#include <string>

class NotificationService {
public:
    NotificationService(
        std::shared_ptr<EmailService>          emailService,
        std::shared_ptr<IEmployeeRepository>   employeeRepository,
        std::shared_ptr<ITimesheetRepository>  timesheetRepository,
        std::shared_ptr<IProjectRepository>    projectRepository,
        std::shared_ptr<IMilestoneRepository>  milestoneRepository,
        std::shared_ptr<IAllocationRepository> allocationRepository,
        std::shared_ptr<ILLMAdapter>           llmAdapter
    );

    // Called by scheduler every cycle
    void processTimesheetReminders();

    // Called by scheduler when a project health transitions to AT_RISK
    void sendAtRiskAlert(int projectId);

private:
    // Reminder helpers
    void sendReminder1(int userId, const std::string& weekStart,
                       const std::string& email, const std::string& fullName);
    void sendReminder2(int userId, const std::string& weekStart,
                       const std::string& email, const std::string& fullName);
    void freezeAccount (int userId, const std::string& weekStart,
                        const Employee& employee);

    // DB helpers for reminder_log
    struct ReminderLog {
        bool     exists{false};
        std::string reminder1SentAt;
        std::string reminder2SentAt;
        std::string frozenAt;
    };
    ReminderLog getLog(int userId, const std::string& weekStart);
    void        upsertLog(int userId, const std::string& weekStart,
                          const std::string& field, const std::string& ts);

    static std::string lastMondayString();
    static std::string nowString();
    static bool        isNextDayOrLater(const std::string& isoDatetime);

    // AI helpers for at-risk
    std::string buildAtRiskPrompt(const std::string& projectName,
                                  const std::string& milestonesJson,
                                  const std::string& allocationsJson);

    std::shared_ptr<EmailService>          emailService_;
    std::shared_ptr<IEmployeeRepository>   employeeRepository_;
    std::shared_ptr<ITimesheetRepository>  timesheetRepository_;
    std::shared_ptr<IProjectRepository>    projectRepository_;
    std::shared_ptr<IMilestoneRepository>  milestoneRepository_;
    std::shared_ptr<IAllocationRepository> allocationRepository_;
    std::shared_ptr<ILLMAdapter>           llmAdapter_;
};
