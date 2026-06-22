#pragma once
#include "ITimesheetRepository.hpp"
#include <gmock/gmock.h>

class MockTimesheetRepository : public ITimesheetRepository {
public:
    MOCK_METHOD(std::optional<Timesheet>,    findById,          (int timesheetId), (override));
    MOCK_METHOD(std::optional<Timesheet>,    findByUserAndWeek, (int userId, const std::string& weekStart), (override));
    MOCK_METHOD(std::vector<Timesheet>,      findByUserId,      (int userId), (override));
    MOCK_METHOD(std::vector<Timesheet>,      findByManagerTeam, (int managerUserId, const std::string& weekStart), (override));
    MOCK_METHOD(std::vector<Timesheet>,      findMissedForWeek, (const std::string& weekStart), (override));
    MOCK_METHOD(int,                         create,            (const Timesheet& timesheet), (override));
    MOCK_METHOD(void,                        createMissed,      (int userId, const std::string& weekStart), (override));
    MOCK_METHOD(void,                        updateStatus,      (int timesheetId, const std::string& status), (override));
    MOCK_METHOD(std::vector<TimesheetEntry>, getEntries,        (int timesheetId), (override));
    MOCK_METHOD(void,                        addEntry,          (const TimesheetEntry& entry), (override));
};
