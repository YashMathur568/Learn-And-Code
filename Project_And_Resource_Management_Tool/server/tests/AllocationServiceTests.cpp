#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "services/AllocationService.hpp"
#include "utils/AppException.hpp"
#include "utils/DateUtils.hpp"
#include "mocks/MockAllocationRepository.hpp"
#include "mocks/MockEmployeeRepository.hpp"
#include "mocks/MockProjectRepository.hpp"

using ::testing::Return;
using ::testing::_;
using ::testing::NiceMock;

// ── Helpers ───────────────────────────────────────────────────────────────────

// Returns a project that belongs to manager 1, active dates spanning 2 years
static Project makeActiveProject(int managerId = 1) {
    Project proj;
    proj.projectId = 10;
    proj.managerId = managerId;
    proj.startDate = "2025-01-01";
    proj.endDate   = "2030-12-31";
    proj.status    = "ACTIVE";
    return proj;
}

static Employee makeActiveEmployee(int userId = 5) {
    Employee emp;
    emp.userId   = userId;
    emp.isActive = true;
    emp.status   = "BENCH";
    return emp;
}

// A valid allocation request that should succeed with the fixtures above
static CreateAllocationRequest validRequest() {
    CreateAllocationRequest req;
    req.userId      = 5;
    req.projectId   = 10;
    req.utilisation = 50;
    req.fromDate    = "2027-01-05";  // a Monday, inside project range
    req.toDate      = "2027-06-30";
    return req;
}

// ── Fixture ───────────────────────────────────────────────────────────────────

class AllocationServiceTest : public ::testing::Test {
protected:
    std::shared_ptr<MockAllocationRepository> allocRepo;
    std::shared_ptr<MockEmployeeRepository>   empRepo;
    std::shared_ptr<MockProjectRepository>    projRepo;
    std::unique_ptr<AllocationService>        service;

    void SetUp() override {
        allocRepo = std::make_shared<NiceMock<MockAllocationRepository>>();
        empRepo   = std::make_shared<NiceMock<MockEmployeeRepository>>();
        projRepo  = std::make_shared<NiceMock<MockProjectRepository>>();
        service   = std::make_unique<AllocationService>(allocRepo, empRepo, projRepo);
    }
};

// ── createAllocation: input validation ───────────────────────────────────────

TEST_F(AllocationServiceTest, CreateAllocation_ZeroUtilisation_ThrowsValidation) {
    auto req = validRequest();
    req.utilisation = 0;
    EXPECT_THROW(service->createAllocation(1, req), ValidationException);
}

TEST_F(AllocationServiceTest, CreateAllocation_OverHundredUtilisation_ThrowsValidation) {
    auto req = validRequest();
    req.utilisation = 101;
    EXPECT_THROW(service->createAllocation(1, req), ValidationException);
}

TEST_F(AllocationServiceTest, CreateAllocation_InvalidFromDate_ThrowsValidation) {
    auto req = validRequest();
    req.fromDate = "not-a-date";
    EXPECT_THROW(service->createAllocation(1, req), ValidationException);
}

TEST_F(AllocationServiceTest, CreateAllocation_InvalidToDate_ThrowsValidation) {
    auto req = validRequest();
    req.toDate = "2027-13-99";
    EXPECT_THROW(service->createAllocation(1, req), ValidationException);
}

TEST_F(AllocationServiceTest, CreateAllocation_FromDateNotBeforeToDate_ThrowsValidation) {
    auto req = validRequest();
    req.fromDate = "2027-06-30";
    req.toDate   = "2027-01-05";
    EXPECT_THROW(service->createAllocation(1, req), ValidationException);
}

TEST_F(AllocationServiceTest, CreateAllocation_EqualDates_ThrowsValidation) {
    auto req = validRequest();
    req.fromDate = req.toDate = "2027-03-01";
    EXPECT_THROW(service->createAllocation(1, req), ValidationException);
}

// ── createAllocation: project checks ─────────────────────────────────────────

TEST_F(AllocationServiceTest, CreateAllocation_ProjectNotFound_ThrowsNotFound) {
    EXPECT_CALL(*projRepo, findById(10)).WillOnce(Return(std::nullopt));
    EXPECT_THROW(service->createAllocation(1, validRequest()), NotFoundException);
}

TEST_F(AllocationServiceTest, CreateAllocation_WrongManager_ThrowsUnauthorized) {
    auto proj = makeActiveProject(/*managerId=*/99);
    EXPECT_CALL(*projRepo, findById(10)).WillOnce(Return(proj));
    EXPECT_THROW(service->createAllocation(1, validRequest()), UnauthorizedException);
}

TEST_F(AllocationServiceTest, CreateAllocation_FromDateBeforeProjectStart_ThrowsValidation) {
    auto proj = makeActiveProject();
    proj.startDate = "2027-06-01";  // request fromDate is 2027-01-05 — before start
    EXPECT_CALL(*projRepo, findById(10)).WillOnce(Return(proj));
    EXPECT_THROW(service->createAllocation(1, validRequest()), ValidationException);
}

TEST_F(AllocationServiceTest, CreateAllocation_ToDdateAfterProjectEnd_ThrowsValidation) {
    auto proj = makeActiveProject();
    proj.endDate = "2027-03-01";  // request toDate is 2027-06-30 — after project end
    EXPECT_CALL(*projRepo, findById(10)).WillOnce(Return(proj));
    EXPECT_THROW(service->createAllocation(1, validRequest()), ValidationException);
}

// ── createAllocation: employee checks ────────────────────────────────────────

TEST_F(AllocationServiceTest, CreateAllocation_EmployeeNotFound_ThrowsNotFound) {
    EXPECT_CALL(*projRepo,  findById(10)).WillOnce(Return(makeActiveProject()));
    EXPECT_CALL(*empRepo,   findById(5)).WillOnce(Return(std::nullopt));
    EXPECT_THROW(service->createAllocation(1, validRequest()), NotFoundException);
}

TEST_F(AllocationServiceTest, CreateAllocation_InactiveEmployee_ThrowsValidation) {
    auto emp = makeActiveEmployee();
    emp.isActive = false;
    EXPECT_CALL(*projRepo, findById(10)).WillOnce(Return(makeActiveProject()));
    EXPECT_CALL(*empRepo,  findById(5)).WillOnce(Return(emp));
    EXPECT_THROW(service->createAllocation(1, validRequest()), ValidationException);
}

TEST_F(AllocationServiceTest, CreateAllocation_WouldExceed100Percent_ThrowsValidation) {
    EXPECT_CALL(*projRepo,  findById(10)).WillOnce(Return(makeActiveProject()));
    EXPECT_CALL(*empRepo,   findById(5)).WillOnce(Return(makeActiveEmployee()));
    EXPECT_CALL(*allocRepo, getTotalActiveUtilisation(5)).WillOnce(Return(60));
    // 60 + 50 = 110 > 100 → should throw
    EXPECT_THROW(service->createAllocation(1, validRequest()), ValidationException);
}

// ── createAllocation: success path ───────────────────────────────────────────

TEST_F(AllocationServiceTest, CreateAllocation_ValidRequest_ReturnsAllocation) {
    EXPECT_CALL(*projRepo,  findById(10)).WillOnce(Return(makeActiveProject()));
    EXPECT_CALL(*empRepo,   findById(5)).WillOnce(Return(makeActiveEmployee()));
    EXPECT_CALL(*allocRepo, getTotalActiveUtilisation(5)).WillOnce(Return(30));
    EXPECT_CALL(*allocRepo, create(_)).WillOnce(Return(42));
    EXPECT_CALL(*empRepo,   setStatus(5, "ALLOCATED"));

    const Allocation result = service->createAllocation(1, validRequest());
    EXPECT_EQ(result.allocationId, 42);
    EXPECT_EQ(result.userId,       5);
    EXPECT_EQ(result.projectId,    10);
    EXPECT_EQ(result.utilisation,  50);
    EXPECT_TRUE(result.isActive);
}

// ── endAllocation ─────────────────────────────────────────────────────────────

TEST_F(AllocationServiceTest, EndAllocation_NotFound_ThrowsNotFound) {
    EXPECT_CALL(*allocRepo, findById(1)).WillOnce(Return(std::nullopt));
    EXPECT_THROW(service->endAllocation(1, 1), NotFoundException);
}

TEST_F(AllocationServiceTest, EndAllocation_AlreadyEnded_ThrowsValidation) {
    Allocation alloc;
    alloc.allocationId = 1;
    alloc.isActive     = false;
    EXPECT_CALL(*allocRepo, findById(1)).WillOnce(Return(alloc));
    EXPECT_THROW(service->endAllocation(1, 1), ValidationException);
}

TEST_F(AllocationServiceTest, EndAllocation_WrongManager_ThrowsUnauthorized) {
    Allocation alloc;
    alloc.allocationId = 1;
    alloc.projectId    = 10;
    alloc.isActive     = true;
    EXPECT_CALL(*allocRepo, findById(1)).WillOnce(Return(alloc));
    auto proj = makeActiveProject(/*managerId=*/99);
    EXPECT_CALL(*projRepo, findById(10)).WillOnce(Return(proj));
    EXPECT_THROW(service->endAllocation(1, /*managerUserId=*/1), UnauthorizedException);
}

TEST_F(AllocationServiceTest, EndAllocation_LastAllocation_SetsStatusToBench) {
    Allocation alloc;
    alloc.allocationId = 1;
    alloc.projectId    = 10;
    alloc.userId       = 5;
    alloc.isActive     = true;
    EXPECT_CALL(*allocRepo, findById(1)).WillOnce(Return(alloc));
    EXPECT_CALL(*projRepo,  findById(10)).WillOnce(Return(makeActiveProject()));
    EXPECT_CALL(*allocRepo, end(1));
    EXPECT_CALL(*empRepo,   hasActiveAllocations(5)).WillOnce(Return(false));
    EXPECT_CALL(*empRepo,   setStatus(5, "BENCH"));

    const Allocation result = service->endAllocation(1, 1);
    EXPECT_FALSE(result.isActive);
}

TEST_F(AllocationServiceTest, EndAllocation_StillHasOtherAllocations_StatusUnchanged) {
    Allocation alloc;
    alloc.allocationId = 1;
    alloc.projectId    = 10;
    alloc.userId       = 5;
    alloc.isActive     = true;
    EXPECT_CALL(*allocRepo, findById(1)).WillOnce(Return(alloc));
    EXPECT_CALL(*projRepo,  findById(10)).WillOnce(Return(makeActiveProject()));
    EXPECT_CALL(*allocRepo, end(1));
    EXPECT_CALL(*empRepo,   hasActiveAllocations(5)).WillOnce(Return(true));
    EXPECT_CALL(*empRepo,   setStatus(_, _)).Times(0);  // must NOT change status

    service->endAllocation(1, 1);
}

// ── Delegation methods ────────────────────────────────────────────────────────

TEST_F(AllocationServiceTest, GetByUserId_DelegatesToRepository) {
    std::vector<Allocation> expected = {{}, {}};
    EXPECT_CALL(*allocRepo, findByUserId(7)).WillOnce(Return(expected));
    EXPECT_EQ(service->getByUserId(7).size(), 2u);
}

TEST_F(AllocationServiceTest, GetActiveByUserId_DelegatesToRepository) {
    EXPECT_CALL(*allocRepo, findActiveByUserId(7)).WillOnce(Return(std::vector<Allocation>{}));
    EXPECT_TRUE(service->getActiveByUserId(7).empty());
}

TEST_F(AllocationServiceTest, GetActiveByProjectId_DelegatesToRepository) {
    EXPECT_CALL(*allocRepo, findActiveByProjectId(10)).WillOnce(Return(std::vector<Allocation>{}));
    service->getActiveByProjectId(10);
}

TEST_F(AllocationServiceTest, GetTotalUtilisation_DelegatesToRepository) {
    EXPECT_CALL(*allocRepo, getTotalActiveUtilisation(7)).WillOnce(Return(75));
    EXPECT_EQ(service->getTotalUtilisation(7), 75);
}
