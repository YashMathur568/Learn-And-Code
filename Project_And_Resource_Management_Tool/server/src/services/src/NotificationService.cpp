#include "NotificationService.hpp"
#include "AppException.hpp"

#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/exception.h>
#include <nlohmann/json.hpp>

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <cstring>


NotificationService::NotificationService(
    std::shared_ptr<EmailService>          emailService,
    std::shared_ptr<IResourceRepository>   resourceRepository,
    std::shared_ptr<ITimesheetRepository>  timesheetRepository,
    std::shared_ptr<IProjectRepository>    projectRepository,
    std::shared_ptr<IMilestoneRepository>  milestoneRepository,
    std::shared_ptr<IAllocationRepository> allocationRepository,
    std::shared_ptr<LLMProviderManager>    llmProviderManager
)
    : emailService_(std::move(emailService))
    , resourceRepository_(std::move(resourceRepository))
    , timesheetRepository_(std::move(timesheetRepository))
    , projectRepository_(std::move(projectRepository))
    , milestoneRepository_(std::move(milestoneRepository))
    , allocationRepository_(std::move(allocationRepository))
    , llmProviderManager_(std::move(llmProviderManager)) {}


std::string NotificationService::lastMondayString() {
    const std::time_t now = std::time(nullptr);
    std::tm nowTimeStructure = {};
    localtime_s(&nowTimeStructure, &now);
    const int daysToMon = (nowTimeStructure.tm_wday == 0) ? 6 : (nowTimeStructure.tm_wday - 1);
    const std::time_t monday = now - (daysToMon * 86400);
    std::tm mondayTimeStructure = {};
    localtime_s(&mondayTimeStructure, &monday);
    std::ostringstream ss;
    ss << std::put_time(&mondayTimeStructure, "%Y-%m-%d");
    return ss.str();
}

std::string NotificationService::nowString() {
    const std::time_t now = std::time(nullptr);
    std::tm nowTimeStructure = {};
    localtime_s(&nowTimeStructure, &now);
    std::ostringstream ss;
    ss << std::put_time(&nowTimeStructure, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

bool NotificationService::isNextDayOrLater(const std::string& isoDatetime) {
    if (isoDatetime.size() < 10) return false;
    const std::string sentDate = isoDatetime.substr(0, 10);
    const std::time_t now = std::time(nullptr);
    std::tm nowTimeStructure = {};
    localtime_s(&nowTimeStructure, &now);
    std::ostringstream ss;
    ss << std::put_time(&nowTimeStructure, "%Y-%m-%d");
    return ss.str() > sentDate;
}

NotificationService::ReminderLog
NotificationService::getLog(int userId, const std::string& weekStart) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                "SELECT reminder1_sent_at, reminder2_sent_at, frozen_at "
                "FROM timesheet_reminder_log WHERE user_id = ? AND week_start = ?"
            )
        );
        statement->setInt(1, userId);
        statement->setString(2, weekStart);
        std::unique_ptr<sql::ResultSet> resultSet(statement->executeQuery());
        ReminderLog log;
        if (resultSet->next()) {
            log.exists          = true;
            log.reminder1SentAt = resultSet->isNull("reminder1_sent_at") ? "" : resultSet->getString("reminder1_sent_at").c_str();
            log.reminder2SentAt = resultSet->isNull("reminder2_sent_at") ? "" : resultSet->getString("reminder2_sent_at").c_str();
            log.frozenAt        = resultSet->isNull("frozen_at")         ? "" : resultSet->getString("frozen_at").c_str();
        }
        return log;
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in getLog: ") + sqlException.what());
    }
}

void NotificationService::upsertLog(int userId, const std::string& weekStart,
                                    const std::string& field, const std::string& timesheet) {
    try {
        auto connection = DatabasePool::getInstance().acquire();

        std::unique_ptr<sql::PreparedStatement> insertStatement(
            connection->prepareStatement(
                "INSERT IGNORE INTO timesheet_reminder_log (user_id, week_start) VALUES (?, ?)"
            )
        );
        insertStatement->setInt(1, userId);
        insertStatement->setString(2, weekStart);
        insertStatement->executeUpdate();


        const std::string sqlQuery = "UPDATE timesheet_reminder_log SET " + field +
                                " = ? WHERE user_id = ? AND week_start = ?";
        std::unique_ptr<sql::PreparedStatement> updateStatement(connection->prepareStatement(sqlQuery));
        updateStatement->setString(1, timesheet);
        updateStatement->setInt(2, userId);
        updateStatement->setString(3, weekStart);
        updateStatement->executeUpdate();
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in upsertLog: ") + sqlException.what());
    }
}

void NotificationService::sendReminder1(int userId, const std::string& weekStart,
                                        const std::string& email,
                                        const std::string& fullName) {
    const std::string subject = "[PRM] Timesheet Reminder - Week of " + weekStart;
    const std::string body =
        "Dear " + fullName + ",\n\n"
        "Your timesheet for the week starting " + weekStart + " has not been submitted.\n\n"
        "Please log in and submit it as soon as possible.\n\n"
        "If you do not submit by end of tomorrow, a second reminder will be sent.\n\n"
        " -  PRM Tool";
    emailService_->send(email, subject, body);
    upsertLog(userId, weekStart, "reminder1_sent_at", nowString());
    std::cout << "[Notification] Reminder 1 sent to " << email << " for week " << weekStart << "\n";
}

void NotificationService::sendReminder2(int userId, const std::string& weekStart,
                                        const std::string& email,
                                        const std::string& fullName) {
    const std::string subject = "[PRM] FINAL Reminder - Timesheet Week of " + weekStart;
    const std::string body =
        "Dear " + fullName + ",\n\n"
        "This is your FINAL reminder. Your timesheet for the week of " + weekStart +
        " is still pending.\n\n"
        "If not submitted by end of today, your timesheet submission access will be "
        "FROZEN and your manager will be notified.\n\n"
        " -  PRM Tool";
    emailService_->send(email, subject, body);
    upsertLog(userId, weekStart, "reminder2_sent_at", nowString());
    std::cout << "[Notification] Reminder 2 sent to " << email << " for week " << weekStart << "\n";
}

void NotificationService::freezeAccount(int userId, const std::string& weekStart,
                                        const Resource& employee) {
    resourceRepository_->setFrozen(userId, true);
    upsertLog(userId, weekStart, "frozen_at", nowString());

    // Fetch manager first so we can include their details in the user email
    std::string managerName  = "your manager";
    std::string managerEmail = "";
    std::shared_ptr<Resource> managerPtr;
    if (employee.managerId > 0) {
        auto optManager = resourceRepository_->findById(employee.managerId);
        if (optManager.has_value()) {
            managerName  = optManager->fullName;
            managerEmail = optManager->email;
            managerPtr   = std::make_shared<Resource>(optManager.value());
        }
    }

    // Email to the frozen resource
    const std::string managerContact = managerEmail.empty()
        ? managerName
        : managerName + " (" + managerEmail + ")";

    emailService_->send(
        employee.email,
        "[PRM] Your Timesheet Access Has Been Frozen",
        "Dear " + employee.fullName + ",\n\n"
        "Your timesheet submission access has been FROZEN.\n\n"
        "Reason  : The timesheet for week " + weekStart + " was not submitted\n"
        "          after two automated reminders.\n\n"
        "What this means:\n"
        "  - You can still log in and view your timesheet history.\n"
        "  - You CANNOT submit new timesheets until your access is restored.\n\n"
        "What to do next:\n"
        "  Contact your manager " + managerContact + " and ask them\n"
        "  to restore your access via the PRM Tool.\n\n"
        "- PRM Tool (Automated Alert)"
    );

    // Email to the manager
    if (managerPtr) {
        emailService_->send(
            managerPtr->email,
            "[PRM] ACTION REQUIRED - " + employee.fullName + " Timesheet Access Frozen",
            "Dear " + managerPtr->fullName + ",\n\n"
            "This is an automated alert requiring your attention.\n\n"
            "Resource : " + employee.fullName + " (" + employee.email + ")\n"
            "Week     : " + weekStart + "\n"
            "Reason   : Timesheet not submitted after two automated reminders.\n\n"
            "The resource's timesheet submission access has been FROZEN.\n"
            "They have been notified and instructed to contact you.\n\n"
            "Action required:\n"
            "  Log in to PRM Tool and go to:\n"
            "  Manager Dashboard > Resources > " + employee.fullName + " > Restore Access\n\n"
            "- PRM Tool (Automated Alert)"
        );
    }

    std::cout << "[Notification] Account FROZEN for user " << userId
              << " (" << employee.fullName << ") week " << weekStart << "\n";
}


void NotificationService::processTimesheetReminders() {
    const std::string lastWeek = lastMondayString();

    const std::time_t now = std::time(nullptr);
    std::tm nowTimeStructure = {};
    localtime_s(&nowTimeStructure, &now);
    const int daysToLastMonday = (nowTimeStructure.tm_wday == 0) ? 6 : (nowTimeStructure.tm_wday - 1);
    const std::time_t prevMondayTime = now - ((daysToLastMonday + 7) * 86400);
    std::tm previousMondayTimeStructure = {};
    localtime_s(&previousMondayTimeStructure, &prevMondayTime);
    std::ostringstream ss;
    ss << std::put_time(&previousMondayTimeStructure, "%Y-%m-%d");
    const std::string prevWeek = ss.str();

    const auto missedTimesheets = timesheetRepository_->findMissedForWeek(prevWeek);

    for (const auto& timesheet : missedTimesheets) {
        const int userId = timesheet.userId;
        auto optEmp = resourceRepository_->findById(userId);
        if (!optEmp.has_value()) continue;
        const Resource& employee = optEmp.value();

        const auto log = getLog(userId, prevWeek);

        if (!log.exists || log.reminder1SentAt.empty()) {

            sendReminder1(userId, prevWeek, employee.email, employee.fullName);
        } else if (log.reminder2SentAt.empty() && isNextDayOrLater(log.reminder1SentAt)) {

            sendReminder2(userId, prevWeek, employee.email, employee.fullName);
        } else if (!log.reminder2SentAt.empty() && log.frozenAt.empty()
                   && isNextDayOrLater(log.reminder2SentAt)) {

            if (!employee.isFrozen) {
                freezeAccount(userId, prevWeek, employee);
            }
        }
    }
}


void NotificationService::sendAtRiskAlert(int projectId) {
    auto optProject = projectRepository_->findById(projectId);
    if (!optProject.has_value()) return;

    const auto& project = optProject.value();

    auto optManager = resourceRepository_->findById(project.managerId);
    if (!optManager.has_value()) return;
    const std::string managerEmail = optManager->email;
    const std::string managerName  = optManager->fullName;


    const auto milestones = milestoneRepository_->findByProjectId(projectId);
    nlohmann::json msJson = nlohmann::json::array();
    for (const auto& milestone : milestones) {
        msJson.push_back({{"title", milestone.title}, {"dueDate", milestone.dueDate}, {"status", milestone.status}});
    }


    const auto allocations = allocationRepository_->findActiveByProjectId(projectId);
    nlohmann::json allocJson = nlohmann::json::array();
    for (const auto& allocation : allocations) {
        auto optEmp = resourceRepository_->findById(allocation.userId);
        if (optEmp.has_value()) {
            allocJson.push_back({
                {"name",        optEmp->fullName},
                {"utilisation", allocation.utilisation},
                {"toDate",      allocation.toDate}
            });
        }
    }


    std::string aiSummary;
    try {
        const std::string prompt = buildAtRiskPrompt(project.name, msJson.dump(2), allocJson.dump(2));
        aiSummary = llmProviderManager_->getCurrentAdapter()->generate(prompt);

        if (aiSummary.size() > 500) aiSummary = aiSummary.substr(0, 500) + "...";
    } catch (...) {
        aiSummary = "(AI summary unavailable)";
    }


    std::string suggestedHelp = "No bench resources currently available.";
    try {
        auto conn = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> stmt(
            conn->prepareStatement(
                "SELECT user.full_name, rp.designation, skill.skill_name "
                "FROM users user "
                "JOIN user_profile rp ON rp.user_id = user.user_id "
                "JOIN resource_status rs ON rs.user_id = user.user_id "
                "JOIN user_skills skill ON skill.user_id = user.user_id "
                "WHERE user.is_active = 1 AND rs.status = 'BENCH' LIMIT 5"
            )
        );
        std::unique_ptr<sql::ResultSet> rs(stmt->executeQuery());
        std::ostringstream help;
        help << "Available bench resources:\n";
        bool any = false;
        while (rs->next()) {
            help << "  • " << rs->getString("full_name")
                 << " (" << rs->getString("designation") << ")"
                 << "  -  " << rs->getString("skill_name") << "\n";
            any = true;
        }
        if (any) suggestedHelp = help.str();
    } catch (...) {}


    std::ostringstream body;
    body << "Dear " << managerName << ",\n\n"
         << "[!]  Project \"" << project.name << "\" has been marked AT_RISK by the system.\n\n"
         << "── Project Details ──────────────────────────────\n"
         << "Status  : " << project.status << "\n"
         << "Health  : AT_RISK\n"
         << "Ends    : " << project.endDate << "\n\n"
         << "── Key Milestones ───────────────────────────────\n";
    for (const auto& milestone : milestones) {
        body << "  [" << milestone.status << "] " << milestone.title << "  -  due " << milestone.dueDate << "\n";
    }
    body << "\n── AI Risk Summary ──────────────────────────────\n"
         << aiSummary << "\n\n"
         << "── Suggested Help ───────────────────────────────\n"
         << suggestedHelp << "\n\n"
         << "Please log in to PRM Tool to take action.\n\n"
         << " -  PRM Tool (Automated Alert)";

    emailService_->send(
        managerEmail,
        "[PRM] [!] Project AT_RISK: " + project.name,
        body.str()
    );
    std::cout << "[Notification] AT_RISK alert sent to " << managerEmail
              << " for project " << project.name << "\n";
}

std::string NotificationService::buildAtRiskPrompt(const std::string& projectName,
                                                    const std::string& milestonesJson,
                                                    const std::string& allocationsJson) {
    return "You are allocation project risk analyst. In 3-4 sentences, explain why the project \"" +
           projectName + "\" is at risk based on the following data.\n\n"
           "Milestones:\n" + milestonesJson + "\n\n"
           "Current Allocations:\n" + allocationsJson + "\n\n"
           "Be concise and actionable. Output plain text only, no markdown.";
}
