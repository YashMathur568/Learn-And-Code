#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <fstream>

#include "services/TimesheetService.hpp"
#include "utils/AppException.hpp"
#include "utils/ConfigLoader.hpp"
#include "mocks/MockTimesheetRepository.hpp"
#include "mocks/MockAllocationRepository.hpp"
#include "mocks/MockEmployeeRepository.hpp"

using ::testing::Return;
using ::testing::_;
using ::testing::AnyNumber;
using ::testing::NiceMock;

// ── Helpers ───────────────────────────────────────────────────────────────────

// Last Monday that is guaranteed to be in the past (2026-06-08 is a Monday)
static const std::string PAST_MONDAY   = "2026-06-08";
// A future Monday
static const std::string FUTURE_MONDAY = "2099-01-07";
// A day that is not a Monday
static const std::string NOT_A_MONDAY  = "2026-06-09";  // Tuesday

static SubmitTimesheetRequest makeValidRequest(const std::string& weekStart = PAST_MONDAY) {
    SubmitTimesheetRequest req;
    req.weekStart = weekStart;
    TimesheetEntryRequest entry;
    entry.projectId    = 10;
    entry.hours        = 8;
    entry.activityTags = "Development";
    req.entries.push_back(entry);
    return req;
}

static Timesheet makeStoredTimesheet(int id = 100, int userId = 1) {
    Timesheet ts;
    ts.timesheetId = id;
    ts.userId      = userId;
    ts.weekStart   = PAST_MONDAY;
    ts.status      = "SUBMITTED";
    return ts;
}

// ── Bootstrap: ConfigLoader singleton must be initialised before tests use it.
// We set maxWeeklyHours = 40 by pointing at the real config.json in the build
// directory. If that file is absent we just leave the default (40) baked in.
// ─────────────────────────────────────────────────────────────────────────────

// Helper: force ConfigLoader into the loaded state with default values by
// writing a minimal config.json next to the test binary and loading it once.
static void bootstrapConfigLoader() {
    static bool done = false;
    if (done) return;
    done = true;

    // Write a minimal valid config file to the current working directory
    const char* minimalConfig = R"({
        "database": {"host":"localhost","port":3306,"name":"prm","username":"root","password":""},
        "llm":      {"provider":"gemma","apiKey":"","geminiModel":"","groqModel":"","companyHost":"","companyModel":""},
        "email":    {"enabled":false,"smtpHost":"","smtpPort":587,"username":"","password":"","fromAddress":""},
        "jwtSecret":              "test_secret",
        "serverPort":             8080,
        "schedulerIntervalHours": 4,
        "maxWeeklyHours":         40
    })";

    std::ofstream out("test_config.json");
    if (out.is_open()) {
        out << minimalConfig;
        out.close();
        try { ConfigLoader::getInstance().load("test_config.json"); } catch (...) {}
    }
}

class TimesheetServiceTest : public ::testing::Test {
protected:
    std::shared_ptr<MockTimesheetRepository>  tsRepo;
    std::shared_ptr<MockAllocationRepository> allocRepo;
    std::shared_ptr<MockEmployeeRepository>   empRepo;
    std::unique_ptr<TimesheetService>         service;

    void SetUp() override {
        bootstrapConfigLoader();

        tsRepo    = std::make_shared<NiceMock<MockTimesheetRepository>>();
        allocRepo = std::make_shared<NiceMock<MockAllocationRepository>>();
        empRepo   = std::make_shared<NiceMock<MockEmployeeRepository>>();
        service   = std::make_unique<TimesheetService>(tsRepo, allocRepo, empRepo);
    }
};

// ── submitTimesheet: account-frozen check ────────────────────────────────────

TEST_F(TimesheetServiceTest, Submit_FrozenAccount_ThrowsValidation) {
    EXPECT_CALL(*empRepo, isFrozen(1)).WillOnce(Return(true));
    EXPECT_THROW(service->submitTimesheet(1, makeValidRequest()), ValidationException);
}

// ── submitTimesheet: empty entries ───────────────────────────────────────────

TEST_F(TimesheetServiceTest, Submit_EmptyEntries_ThrowsValidation) {
    EXPECT_CALL(*empRepo, isFrozen(1)).WillOnce(Return(false));
    SubmitTimesheetRequest req;
    req.weekStart = PAST_MONDAY;
    // no entries
    EXPECT_THROW(service->submitTimesheet(1, req), ValidationException);
}

// ── submitTimesheet: weekStart validation ────────────────────────────────────

TEST_F(TimesheetServiceTest, Submit_WeekStartNotMonday_ThrowsValidation) {
    EXPECT_CALL(*empRepo, isFrozen(1)).WillOnce(Return(false));
    EXPECT_THROW(service->submitTimesheet(1, makeValidRequest(NOT_A_MONDAY)), ValidationException);
}

TEST_F(TimesheetServiceTest, Submit_WeekStartInFuture_ThrowsValidation) {
    EXPECT_CALL(*empRepo, isFrozen(1)).WillOnce(Return(false));
    EXPECT_THROW(service->submitTimesheet(1, makeValidRequest(FUTURE_MONDAY)), ValidationException);
}

TEST_F(TimesheetServiceTest, Submit_InvalidDateFormat_ThrowsValidation) {
    EXPECT_CALL(*empRepo, isFrozen(1)).WillOnce(Return(false));
    EXPECT_THROW(service->submitTimesheet(1, makeValidRequest("not-a-date")), ValidationException);
}

// ── submitTimesheet: duplicate check ─────────────────────────────────────────

TEST_F(TimesheetServiceTest, Submit_AlreadySubmittedThisWeek_ThrowsConflict) {
    EXPECT_CALL(*empRepo, isFrozen(1)).WillOnce(Return(false));
    EXPECT_CALL(*tsRepo,  findByUserAndWeek(1, PAST_MONDAY))
        .WillOnce(Return(makeStoredTimesheet()));
    EXPECT_THROW(service->submitTimesheet(1, makeValidRequest()), ConflictException);
}

// ── submitTimesheet: entry-level validation ───────────────────────────────────

TEST_F(TimesheetServiceTest, Submit_EntryWithZeroProjectId_ThrowsValidation) {
    EXPECT_CALL(*empRepo, isFrozen(1)).WillOnce(Return(false));
    EXPECT_CALL(*tsRepo,  findByUserAndWeek(1, PAST_MONDAY)).WillOnce(Return(std::nullopt));

    SubmitTimesheetRequest req;
    req.weekStart = PAST_MONDAY;
    TimesheetEntryRequest badEntry;
    badEntry.projectId = 0;
    badEntry.hours     = 8;
    req.entries.push_back(badEntry);
    EXPECT_THROW(service->submitTimesheet(1, req), ValidationException);
}

TEST_F(TimesheetServiceTest, Submit_EntryWithZeroHours_ThrowsValidation) {
    EXPECT_CALL(*empRepo, isFrozen(1)).WillOnce(Return(false));
    EXPECT_CALL(*tsRepo,  findByUserAndWeek(1, PAST_MONDAY)).WillOnce(Return(std::nullopt));

    SubmitTimesheetRequest req;
    req.weekStart = PAST_MONDAY;
    TimesheetEntryRequest badEntry;
    badEntry.projectId = 10;
    badEntry.hours     = 0;
    req.entries.push_back(badEntry);
    EXPECT_THROW(service->submitTimesheet(1, req), ValidationException);
}

TEST_F(TimesheetServiceTest, Submit_NotAllocatedToProject_ThrowsValidation) {
    EXPECT_CALL(*empRepo,   isFrozen(1)).WillOnce(Return(false));
    EXPECT_CALL(*tsRepo,    findByUserAndWeek(1, PAST_MONDAY)).WillOnce(Return(std::nullopt));
    EXPECT_CALL(*allocRepo, isActivelyAllocated(1, 10)).WillOnce(Return(false));

    EXPECT_THROW(service->submitTimesheet(1, makeValidRequest()), ValidationException);
}

TEST_F(TimesheetServiceTest, Submit_TotalHoursExceedWeeklyCap_ThrowsValidation) {
    EXPECT_CALL(*empRepo,   isFrozen(1)).WillOnce(Return(false));
    EXPECT_CALL(*tsRepo,    findByUserAndWeek(1, PAST_MONDAY)).WillOnce(Return(std::nullopt));
    EXPECT_CALL(*allocRepo, isActivelyAllocated(1, 10)).WillOnce(Return(true));

    SubmitTimesheetRequest req;
    req.weekStart = PAST_MONDAY;
    TimesheetEntryRequest bigEntry;
    bigEntry.projectId = 10;
    bigEntry.hours     = 99;   // way over 40-hour cap
    req.entries.push_back(bigEntry);
    EXPECT_THROW(service->submitTimesheet(1, req), ValidationException);
}

// ── submitTimesheet: success path ────────────────────────────────────────────

TEST_F(TimesheetServiceTest, Submit_ValidRequest_CreatesTimesheetAndEntries) {
    const int newTimesheetId = 100;
    Timesheet storedTs       = makeStoredTimesheet(newTimesheetId, 1);
    TimesheetEntry storedEntry;
    storedEntry.entryId      = 1;
    storedEntry.timesheetId  = newTimesheetId;
    storedEntry.projectId    = 10;
    storedEntry.hours        = 8;

    EXPECT_CALL(*empRepo,   isFrozen(1)).WillOnce(Return(false));
    EXPECT_CALL(*tsRepo,    findByUserAndWeek(1, PAST_MONDAY)).WillOnce(Return(std::nullopt));
    EXPECT_CALL(*allocRepo, isActivelyAllocated(1, 10)).WillOnce(Return(true));
    EXPECT_CALL(*tsRepo,    create(_)).WillOnce(Return(newTimesheetId));
    EXPECT_CALL(*tsRepo,    findById(newTimesheetId)).WillOnce(Return(storedTs));
    EXPECT_CALL(*tsRepo,    addEntry(_));
    EXPECT_CALL(*tsRepo,    getEntries(newTimesheetId))
        .WillOnce(Return(std::vector<TimesheetEntry>{storedEntry}));

    const TimesheetWithEntries result = service->submitTimesheet(1, makeValidRequest());
    EXPECT_EQ(result.timesheet.timesheetId, newTimesheetId);
    EXPECT_EQ(result.timesheet.status,      "SUBMITTED");
    EXPECT_EQ(result.entries.size(),        1u);
    EXPECT_EQ(result.entries[0].hours,      8);
}

// ── getByUserId ───────────────────────────────────────────────────────────────

TEST_F(TimesheetServiceTest, GetByUserId_ReturnsTimesheetsWithEntries) {
    Timesheet ts = makeStoredTimesheet(1, 5);
    TimesheetEntry entry;
    entry.entryId     = 1;
    entry.timesheetId = 1;

    EXPECT_CALL(*tsRepo, findByUserId(5)).WillOnce(Return(std::vector<Timesheet>{ts}));
    EXPECT_CALL(*tsRepo, getEntries(1)).WillOnce(Return(std::vector<TimesheetEntry>{entry}));

    const auto results = service->getByUserId(5);
    ASSERT_EQ(results.size(), 1u);
    EXPECT_EQ(results[0].timesheet.timesheetId, 1);
    EXPECT_EQ(results[0].entries.size(),         1u);
}

TEST_F(TimesheetServiceTest, GetByUserId_NoTimesheets_ReturnsEmpty) {
    EXPECT_CALL(*tsRepo, findByUserId(5)).WillOnce(Return(std::vector<Timesheet>{}));
    EXPECT_TRUE(service->getByUserId(5).empty());
}

// ── getTeamTimesheets ─────────────────────────────────────────────────────────

TEST_F(TimesheetServiceTest, GetTeamTimesheets_ReturnsTimesheetsWithEntries) {
    Timesheet ts1 = makeStoredTimesheet(10, 2);
    Timesheet ts2 = makeStoredTimesheet(11, 3);

    EXPECT_CALL(*tsRepo, findByManagerTeam(1, PAST_MONDAY))
        .WillOnce(Return(std::vector<Timesheet>{ts1, ts2}));
    EXPECT_CALL(*tsRepo, getEntries(10)).WillOnce(Return(std::vector<TimesheetEntry>{}));
    EXPECT_CALL(*tsRepo, getEntries(11)).WillOnce(Return(std::vector<TimesheetEntry>{}));

    const auto results = service->getTeamTimesheets(1, PAST_MONDAY);
    EXPECT_EQ(results.size(), 2u);
}

TEST_F(TimesheetServiceTest, GetTeamTimesheets_NoResults_ReturnsEmpty) {
    EXPECT_CALL(*tsRepo, findByManagerTeam(1, PAST_MONDAY))
        .WillOnce(Return(std::vector<Timesheet>{}));
    EXPECT_TRUE(service->getTeamTimesheets(1, PAST_MONDAY).empty());
}
