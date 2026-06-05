#include "MySQLTimesheetRepository.hpp"
#include "../utils/AppException.hpp"

#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/exception.h>

#include <memory>

Timesheet MySQLTimesheetRepository::mapRowToTimesheet(sql::ResultSet* resultSet) {
    Timesheet timesheet;
    timesheet.timesheetId  = resultSet->getInt("timesheet_id");
    timesheet.employeeId   = resultSet->getInt("employee_id");
    timesheet.weekStart    = resultSet->getString("week_start").asStdString();
    timesheet.status       = resultSet->getString("status").asStdString();
    timesheet.submittedAt  = resultSet->isNull("submitted_at")
                             ? ""
                             : resultSet->getString("submitted_at").asStdString();
    return timesheet;
}

TimesheetEntry MySQLTimesheetRepository::mapRowToEntry(sql::ResultSet* resultSet) {
    TimesheetEntry entry;
    entry.entryId       = resultSet->getInt("entry_id");
    entry.timesheetId   = resultSet->getInt("timesheet_id");
    entry.projectId     = resultSet->getInt("project_id");
    entry.hours         = resultSet->getInt("hours");
    entry.activityTags  = resultSet->isNull("activity_tags")
                          ? ""
                          : resultSet->getString("activity_tags").asStdString();
    return entry;
}

static const std::string SHEET_SELECT =
    "SELECT timesheet_id, employee_id, week_start, status, submitted_at "
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

std::optional<Timesheet> MySQLTimesheetRepository::findByEmployeeAndWeek(
    int employeeId, const std::string& weekStart
) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(SHEET_SELECT + "WHERE employee_id = ? AND week_start = ?")
        );
        statement->setInt(1, employeeId);
        statement->setString(2, weekStart);
        std::unique_ptr<sql::ResultSet> resultSet(statement->executeQuery());
        if (resultSet->next()) {
            return mapRowToTimesheet(resultSet.get());
        }
        return std::nullopt;
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in findByEmployeeAndWeek: ") + sqlException.what());
    }
}

std::vector<Timesheet> MySQLTimesheetRepository::findByEmployeeId(int employeeId) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(SHEET_SELECT + "WHERE employee_id = ? ORDER BY week_start DESC")
        );
        statement->setInt(1, employeeId);
        std::unique_ptr<sql::ResultSet> resultSet(statement->executeQuery());
        std::vector<Timesheet> timesheets;
        while (resultSet->next()) {
            timesheets.push_back(mapRowToTimesheet(resultSet.get()));
        }
        return timesheets;
    } catch (const sql::SQLException& sqlException) {
        throw AppException(std::string("DB error in timesheet findByEmployeeId: ") + sqlException.what());
    }
}

std::vector<Timesheet> MySQLTimesheetRepository::findByManagerTeam(
    int managerEmployeeId, const std::string& weekStart
) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                "SELECT t.timesheet_id, t.employee_id, t.week_start, t.status, t.submitted_at "
                "FROM timesheets t "
                "JOIN employees e ON t.employee_id = e.employee_id "
                "WHERE e.manager_id = ? AND t.week_start = ? "
                "ORDER BY t.employee_id"
            )
        );
        statement->setInt(1, managerEmployeeId);
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

int MySQLTimesheetRepository::create(const Timesheet& timesheet) {
    try {
        auto connection = DatabasePool::getInstance().acquire();
        std::unique_ptr<sql::PreparedStatement> statement(
            connection->prepareStatement(
                "INSERT INTO timesheets (employee_id, week_start, status, submitted_at) "
                "VALUES (?, ?, 'SUBMITTED', NOW())"
            )
        );
        statement->setInt(1, timesheet.employeeId);
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
                "SELECT entry_id, timesheet_id, project_id, hours, activity_tags "
                "FROM timesheet_entries WHERE timesheet_id = ? ORDER BY entry_id"
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
