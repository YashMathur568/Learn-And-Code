#include "MySQLTimesheetRepository.hpp"
#include "AppException.hpp"

#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/exception.h>

#include <memory>

Timesheet MySQLTimesheetRepository::mapRowToTimesheet(sql::ResultSet* resultSet) {
    Timesheet timesheet;
    timesheet.timesheetId  = resultSet->getInt("timesheet_id");
    timesheet.userId       = resultSet->getInt("user_id");
    timesheet.weekStart    = resultSet->getString("week_start");
    timesheet.status       = resultSet->getString("status");
    timesheet.submittedAt  = resultSet->isNull("submitted_at")
                             ? ""
                             : resultSet->getString("submitted_at");
    return timesheet;
}

TimesheetEntry MySQLTimesheetRepository::mapRowToEntry(sql::ResultSet* resultSet) {
    TimesheetEntry entry;
    entry.entryId       = resultSet->getInt("entry_id");
    entry.timesheetId   = resultSet->getInt("timesheet_id");
    entry.projectId     = resultSet->getInt("project_id");
    entry.projectName   = resultSet->isNull("project_name") ? "" : resultSet->getString("project_name").c_str();
    entry.hours         = resultSet->getInt("hours");
    entry.activityTags  = resultSet->isNull("activity_tags")
                          ? ""
                          : resultSet->getString("activity_tags");
    return entry;
}

static const std::string SHEET_SELECT =
    "SELECT timesheet_id, user_id, week_start, status, submitted_at "
    "FROM timesheets ";

std::optional<Timesheet> MySQLTimesheetRepository::findById(int timesheetId) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(SHEET_SELECT + "WHERE timesheet_id = ?")
        );
        statement->setInt(1, timesheetId);
        std::unique_ptr<sql::ResultSet> resultSet(statement->executeQuery());
        if (resultSet->next()) {
            return mapRowToTimesheet(resultSet.get());
        }
        return std::nullopt;
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in timesheet findById: ") + sqlException.what());
    }
}

std::optional<Timesheet> MySQLTimesheetRepository::findByUserAndWeek(
    int userId, const std::string& weekStart
) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(SHEET_SELECT + "WHERE user_id = ? AND week_start = ?")
        );
        statement->setInt(1, userId);
        statement->setString(2, weekStart);
        std::unique_ptr<sql::ResultSet> resultSet(statement->executeQuery());
        if (resultSet->next()) {
            return mapRowToTimesheet(resultSet.get());
        }
        return std::nullopt;
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in findByUserAndWeek: ") + sqlException.what());
    }
}

std::vector<Timesheet> MySQLTimesheetRepository::findByUserId(int userId) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(SHEET_SELECT + "WHERE user_id = ? ORDER BY week_start DESC")
        );
        statement->setInt(1, userId);
        std::unique_ptr<sql::ResultSet> resultSet(statement->executeQuery());
        std::vector<Timesheet> timesheets;
        while (resultSet->next()) {
            timesheets.push_back(mapRowToTimesheet(resultSet.get()));
        }
        return timesheets;
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in timesheet findByUserId: ") + sqlException.what());
    }
}

std::vector<Timesheet> MySQLTimesheetRepository::findByManagerTeam(
    int managerUserId, const std::string& weekStart
) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                "SELECT t.timesheet_id, t.user_id, t.week_start, t.status, t.submitted_at "
                "FROM timesheets t "
                "JOIN user_profile rp ON t.user_id = rp.user_id "
                "WHERE rp.manager_id = ? AND t.week_start = ? "
                "ORDER BY t.user_id"
            )
        );
        statement->setInt(1, managerUserId);
        statement->setString(2, weekStart);
        std::unique_ptr<sql::ResultSet> resultSet(statement->executeQuery());
        std::vector<Timesheet> timesheets;
        while (resultSet->next()) {
            timesheets.push_back(mapRowToTimesheet(resultSet.get()));
        }
        return timesheets;
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in findByManagerTeam: ") + sqlException.what());
    }
}

std::vector<Timesheet> MySQLTimesheetRepository::findMissedForWeek(const std::string& weekStart) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                "SELECT timesheet_id, user_id, week_start, status, submitted_at "
                "FROM timesheets WHERE week_start = ? AND status = 'MISSED'"
            )
        );
        statement->setString(1, weekStart);
        std::unique_ptr<sql::ResultSet> resultSet(statement->executeQuery());
        std::vector<Timesheet> timesheets;
        while (resultSet->next()) {
            timesheets.push_back(mapRowToTimesheet(resultSet.get()));
        }
        return timesheets;
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in findMissedForWeek: ") + sqlException.what());
    }
}

int MySQLTimesheetRepository::create(const Timesheet& timesheet) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                "INSERT INTO timesheets (user_id, week_start, status, submitted_at) "
                "VALUES (?, ?, 'SUBMITTED', NOW())"
            )
        );
        statement->setInt(1, timesheet.userId);
        statement->setString(2, timesheet.weekStart);
        statement->executeUpdate();

        std::unique_ptr<sql::PreparedStatement> idStatement(
            connection->prepareStatement("SELECT LAST_INSERT_ID()")
        );
        std::unique_ptr<sql::ResultSet> idResult(idStatement->executeQuery());
        idResult->next();
        return idResult->getInt(1);
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in timesheet create: ") + sqlException.what());
    }
}

std::vector<TimesheetEntry> MySQLTimesheetRepository::getEntries(int timesheetId) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                "SELECT te.entry_id, te.timesheet_id, te.project_id, p.name AS project_name, te.hours, te.activity_tags "
                "FROM timesheet_entries te JOIN projects p ON p.project_id = te.project_id "
                "WHERE te.timesheet_id = ? ORDER BY te.entry_id"
            )
        );
        statement->setInt(1, timesheetId);
        std::unique_ptr<sql::ResultSet> resultSet(statement->executeQuery());
        std::vector<TimesheetEntry> entries;
        while (resultSet->next()) {
            entries.push_back(mapRowToEntry(resultSet.get()));
        }
        return entries;
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in getEntries: ") + sqlException.what());
    }
}

void MySQLTimesheetRepository::addEntry(const TimesheetEntry& entry) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                "INSERT INTO timesheet_entries (timesheet_id, project_id, hours, activity_tags) "
                "VALUES (?, ?, ?, ?)"
            )
        );
        statement->setInt(1, entry.timesheetId);
        statement->setInt(2, entry.projectId);
        statement->setInt(3, entry.hours);
        statement->setString(4, entry.activityTags);
        statement->executeUpdate();
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in addEntry: ") + sqlException.what());
    }
}

void MySQLTimesheetRepository::createMissed(int userId, const std::string& weekStart) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                "INSERT IGNORE INTO timesheets (user_id, week_start, status, submitted_at) "
                "VALUES (?, ?, 'MISSED', NULL)"
            )
        );
        statement->setInt(1, userId);
        statement->setString(2, weekStart);
        statement->executeUpdate();
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in createMissed: ") + sqlException.what());
    }
}

void MySQLTimesheetRepository::updateStatus(int timesheetId, const std::string& status) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                "UPDATE timesheets SET status = ?, submitted_at = NOW() WHERE timesheet_id = ?"
            )
        );
        statement->setString(1, status);
        statement->setInt(2, timesheetId);
        statement->executeUpdate();
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in updateStatus: ") + sqlException.what());
    }
}



