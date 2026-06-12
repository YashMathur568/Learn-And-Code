#include "NotificationService.hpp"
#include "../utils/AppException.hpp"

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

// ── Constructor ───────────────────────────────────────────────────────────────
NotificationService::NotificationService(
    std::shared_ptr<EmailService>          emailService,
    std::shared_ptr<IEmployeeRepository>   employeeRepository,
    std::shared_ptr<ITimesheetRepository>  timesheetRepository,
    std::shared_ptr<IProjectRepository>    projectRepository,
    std::shared_ptr<IMilestoneRepository>  milestoneRepository,
    std::shared_ptr<IAllocationRepository> allocationRepository,
    std::shared_ptr<ILLMAdapter>           llmAdapter
)
    : emailService_(std::move(emailService))
    , employeeRepository_(std::move(employeeRepository))
    , timesheetRepository_(std::move(timesheetRepository))
    , projectRepository_(std::move(projectRepository))
    , milestoneRepository_(std::move(milestoneRepository))
    , allocationRepository_(std::move(allocationRepository))
    , llmAdapter_(std::move(llmAdapter)) {}

// ── Helpers ───────────────────────────────────────────────────────────────────
std::string NotificationService::lastMondayString() {
    const std::time_t now = std::time(nullptr);
    std::tm nowTm = {};
    localtime_s(&nowTm, &now);
    const int daysToMon = (nowTm.tm_wday == 0) ? 6 : (nowTm.tm_wday - 1);
    const std::time_t monday = now - (daysToMon * 86400);
    std::tm mondayTm = {};
    localtime_s(&mondayTm, &monday);
    std::ostringstream ss;
    ss << std::put_time(&mondayTm, "%Y-%m-%d");
    return ss.str();
}

std::string NotificationService::nowString() {
    const std::time_t now = std::time(nullptr);
    std::tm nowTm = {};
    localtime_s(&nowTm, &now);
    std::ostringstream ss;
    ss << std::put_time(&nowTm, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// Returns true if the given datetime string is from a PREVIOUS calendar day
bool NotificationService::isNextDayOrLater(const std::string& isoDatetime) {
    if (isoDatetime.size() < 10) return false;
    const std::string sentDate = isoDatetime.substr(0, 10); // "YYYY-MM-DD"
    const std::time_t now = std::time(nullptr);
    std::tm nowTm = {};
    localtime_s(&nowTm, &now);
    std::ostringstream ss;
    ss << std::put_time(&nowTm, "%Y-%m-%d");
    return ss.str() > sentDate;
}

// ── Reminder log DB ops ───────────────────────────────────────────────────────
NotificationService::ReminderLog
NotificationService::getLog(int userId, const std::string& weekStart) {
    try {
        auto conn = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> stmt(
            conn->prepareStatement(
                "SELECT reminder1_sent_at, reminder2_sent_at, frozen_at "
                "FROM timesheet_reminder_log WHERE user_id = ? AND week_start = ?"
            )
        );
        stmt->setInt(1, userId);
        stmt->setString(2, weekStart);
        std::unique_ptr<sql::ResultSet> rs(stmt->executeQuery());
        ReminderLog log;
        if (rs->next()) {
            log.exists          = true;
            log.reminder1SentAt = rs->isNull("reminder1_sent_at") ? "" : rs->getString("reminder1_sent_at").c_str();
            log.reminder2SentAt = rs->isNull("reminder2_sent_at") ? "" : rs->getString("reminder2_sent_at").c_str();
            log.frozenAt        = rs->isNull("frozen_at")         ? "" : rs->getString("frozen_at").c_str();
        }
        return log;
    } catch (const sql::SQLException& ex) {
        throw AppException(std::string("DB error in getLog: ") + ex.what());
    }
}

void NotificationService::upsertLog(int userId, const std::string& weekStart,
                                    const std::string& field, const std::string& ts) {
    try {
        auto conn = DatabasePool::getInstance().acquire();
        // Insert row if not exists
        std::unique_ptr<sql::PreparedStatement> ins(
            conn->prepareStatement(
                "INSERT IGNORE INTO timesheet_reminder_log (user_id, week_start) VALUES (?, ?)"
            )
        );
        ins->setInt(1, userId);
        ins->setString(2, weekStart);
        ins->executeUpdate();

        // Update the specific field
        const std::string sql = "UPDATE timesheet_reminder_log SET " + field +
                                " = ? WHERE user_id = ? AND week_start = ?";
        std::unique_ptr<sql::PreparedStatement> upd(conn->prepareStatement(sql));
        upd->setString(1, ts);
        upd->setInt(2, userId);
        upd->setString(3, weekStart);
        upd->executeUpdate();
    } catch (const sql::SQLException& ex) {
        throw AppException(std::string("DB error in upsertLog: ") + ex.what());
    }
}

// ── Reminder emails ───────────────────────────────────────────────────────────
void NotificationService::sendReminder1(int userId, const std::string& weekStart,
                                        const std::string& email,
                                        const std::string& fullName) {
    const std::string subject = "[PRM] Timesheet Reminder — Week of " + weekStart;
    const std::string body =
        "Dear " + fullName + ",\n\n"
        "Your timesheet for the week starting " + weekStart + " has not been submitted.\n\n"
        "Please log in and submit it as soon as possible.\n\n"
        "If you do not submit by end of tomorrow, a second reminder will be sent.\n\n"
        "— PRM Tool";
    emailService_->send(email, subject, body);
    upsertLog(userId, weekStart, "reminder1_sent_at", nowString());
    std::cout << "[Notification] Reminder 1 sent to " << email << " for week " << weekStart << "\n";
}

void NotificationService::sendReminder2(int userId, const std::string& weekStart,
                                        const std::string& email,
                                        const std::string& fullName) {
    const std::string subject = "[PRM] FINAL Reminder — Timesheet Week of " + weekStart;
    const std::string body =
        "Dear " + fullName + ",\n\n"
        "This is your FINAL reminder. Your timesheet for the week of " + weekStart +
        " is still pending.\n\n"
        "If not submitted by end of today, your timesheet submission access will be "
        "FROZEN and your manager will be notified.\n\n"
        "— PRM Tool";
    emailService_->send(email, subject, body);
    upsertLog(userId, weekStart, "reminder2_sent_at", nowString());
    std::cout << "[Notification] Reminder 2 sent to " << email << " for week " << weekStart << "\n";
}

void NotificationService::freezeAccount(int userId, const std::string& weekStart,
                                        const Employee& emp) {
    // Set frozen in DB
    employeeRepository_->setFrozen(userId, true);
    upsertLog(userId, weekStart, "frozen_at", nowString());

    // Email employee
    emailService_->send(
        emp.email,
        "[PRM] Timesheet Access Frozen",
        "Dear " + emp.fullName + ",\n\n"
        "Your timesheet submission access has been FROZEN because the timesheet for "
        "week " + weekStart + " was not submitted after two reminders.\n\n"
        "You can still log in and view your history, but you cannot submit new timesheets "
        "until your manager restores access.\n\n"
        "Please contact your manager to resolve this.\n\n"
        "— PRM Tool"
    );

    // Email manager
    if (emp.managerId > 0) {
        auto optManager = employeeRepository_->findById(emp.managerId);
        if (optManager.has_value()) {
            emailService_->send(
                optManager->email,
                "[PRM] Employee Timesheet Access Frozen — " + emp.fullName,
                "Dear " + optManager->fullName + ",\n\n"
                "This is to inform you that " + emp.fullName + " (" + emp.email + ") "
                "has had their timesheet submission access FROZEN.\n\n"
                "Reason: Timesheet for week " + weekStart + " was not submitted "
                "after two automated reminders.\n\n"
                "You can restore their access via: Admin/Manager → Employees → Restore Access.\n\n"
                "— PRM Tool"
            );
        }
    }

    std::cout << "[Notification] Account FROZEN for user " << userId
              << " (" << emp.fullName << ") week " << weekStart << "\n";
}

// ── Notification 1: processTimesheetReminders ─────────────────────────────────
void NotificationService::processTimesheetReminders() {
    const std::string lastWeek = lastMondayString();
    // Get the Monday BEFORE last week (last completed week)
    const std::time_t now = std::time(nullptr);
    std::tm nowTm = {};
    localtime_s(&nowTm, &now);
    const int daysToLastMonday = (nowTm.tm_wday == 0) ? 6 : (nowTm.tm_wday - 1);
    const std::time_t prevMondayTime = now - ((daysToLastMonday + 7) * 86400);
    std::tm prevMondayTm = {};
    localtime_s(&prevMondayTm, &prevMondayTime);
    std::ostringstream ss;
    ss << std::put_time(&prevMondayTm, "%Y-%m-%d");
    const std::string prevWeek = ss.str();

    const auto missedTimesheets = timesheetRepository_->findMissedForWeek(prevWeek);

    for (const auto& ts : missedTimesheets) {
        const int userId = ts.userId;
        auto optEmp = employeeRepository_->findById(userId);
        if (!optEmp.has_value()) continue;
        const Employee& emp = optEmp.value();

        const auto log = getLog(userId, prevWeek);

        if (!log.exists || log.reminder1SentAt.empty()) {
            // Send reminder 1
            sendReminder1(userId, prevWeek, emp.email, emp.fullName);
        } else if (log.reminder2SentAt.empty() && isNextDayOrLater(log.reminder1SentAt)) {
            // Send reminder 2
            sendReminder2(userId, prevWeek, emp.email, emp.fullName);
        } else if (!log.reminder2SentAt.empty() && log.frozenAt.empty()
                   && isNextDayOrLater(log.reminder2SentAt)) {
            // Freeze
            if (!emp.isFrozen) {
                freezeAccount(userId, prevWeek, emp);
            }
        }
    }
}

// ── Notification 2: At-Risk Alert ─────────────────────────────────────────────
void NotificationService::sendAtRiskAlert(int projectId) {
    auto optProject = projectRepository_->findById(projectId);
    if (!optProject.has_value()) return;

    const auto& project = optProject.value();

    auto optManager = employeeRepository_->findById(project.managerId);
    if (!optManager.has_value()) return;
    const std::string managerEmail = optManager->email;
    const std::string managerName  = optManager->fullName;

    // Build milestones summary
    const auto milestones = milestoneRepository_->findByProjectId(projectId);
    nlohmann::json msJson = nlohmann::json::array();
    for (const auto& m : milestones) {
        msJson.push_back({{"title", m.title}, {"dueDate", m.dueDate}, {"status", m.status}});
    }

    // Build active allocations summary
    const auto allocations = allocationRepository_->findActiveByProjectId(projectId);
    nlohmann::json allocJson = nlohmann::json::array();
    for (const auto& a : allocations) {
        auto optEmp = employeeRepository_->findById(a.userId);
        if (optEmp.has_value()) {
            allocJson.push_back({
                {"name",        optEmp->fullName},
                {"utilisation", a.utilisation},
                {"toDate",      a.toDate}
            });
        }
    }

    // AI summary
    std::string aiSummary;
    try {
        const std::string prompt = buildAtRiskPrompt(project.name, msJson.dump(2), allocJson.dump(2));
        aiSummary = llmAdapter_->generate(prompt);
        // Trim to first 500 chars for email
        if (aiSummary.size() > 500) aiSummary = aiSummary.substr(0, 500) + "...";
    } catch (...) {
        aiSummary = "(AI summary unavailable)";
    }

    // Find bench employees to suggest
    std::string suggestedHelp = "No bench employees currently available.";
    try {
        auto conn = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> stmt(
            conn->prepareStatement(
                "SELECT u.full_name, rp.designation, s.skill_name "
                "FROM users u "
                "JOIN resource_profile rp ON rp.user_id = u.user_id "
                "JOIN resource_status rs ON rs.user_id = u.user_id "
                "JOIN user_skills s ON s.user_id = u.user_id "
                "WHERE u.is_active = 1 AND rs.status = 'BENCH' LIMIT 5"
            )
        );
        std::unique_ptr<sql::ResultSet> rs(stmt->executeQuery());
        std::ostringstream help;
        help << "Available bench employees:\n";
        bool any = false;
        while (rs->next()) {
            help << "  • " << rs->getString("full_name")
                 << " (" << rs->getString("designation") << ")"
                 << " — " << rs->getString("skill_name") << "\n";
            any = true;
        }
        if (any) suggestedHelp = help.str();
    } catch (...) {}

    // Compose email
    std::ostringstream body;
    body << "Dear " << managerName << ",\n\n"
         << "⚠  Project \"" << project.name << "\" has been marked AT_RISK by the system.\n\n"
         << "── Project Details ──────────────────────────────\n"
         << "Status  : " << project.status << "\n"
         << "Health  : AT_RISK\n"
         << "Ends    : " << project.endDate << "\n\n"
         << "── Key Milestones ───────────────────────────────\n";
    for (const auto& m : milestones) {
        body << "  [" << m.status << "] " << m.title << " — due " << m.dueDate << "\n";
    }
    body << "\n── AI Risk Summary ──────────────────────────────\n"
         << aiSummary << "\n\n"
         << "── Suggested Help ───────────────────────────────\n"
         << suggestedHelp << "\n\n"
         << "Please log in to PRM Tool to take action.\n\n"
         << "— PRM Tool (Automated Alert)";

    emailService_->send(
        managerEmail,
        "[PRM] ⚠ Project AT_RISK: " + project.name,
        body.str()
    );
    std::cout << "[Notification] AT_RISK alert sent to " << managerEmail
              << " for project " << project.name << "\n";
}

std::string NotificationService::buildAtRiskPrompt(const std::string& projectName,
                                                    const std::string& milestonesJson,
                                                    const std::string& allocationsJson) {
    return "You are a project risk analyst. In 3-4 sentences, explain why the project \"" +
           projectName + "\" is at risk based on the following data.\n\n"
           "Milestones:\n" + milestonesJson + "\n\n"
           "Current Allocations:\n" + allocationsJson + "\n\n"
           "Be concise and actionable. Output plain text only, no markdown.";
}
