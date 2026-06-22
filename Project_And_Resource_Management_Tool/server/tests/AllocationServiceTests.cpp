#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "AllocationService.hpp"
#include "AppException.hpp"
#include "DateUtils.hpp"
#include "mocks/MockAllocationRepository.hpp"
#include "mocks/MockResourceRepository.hpp"
#include "mocks/MockProjectRepository.hpp"

using ::testing::Return;
using ::testing::_;
using ::testing::NiceMock;




static Project makeActiveProject(int managerId = 1) {
    Project proj;
    proj.projectId = 10;
    proj.managerId = managerId;
    proj.startDate = "2025-01-01";
    proj.endDate   = "2030-12-31";
    proj.status    = "ACTIVE";
    return proj;
}

static Resource makeActiveResource(int userId = 5) {
    Resource res;
    res.userId   = userId;
    res.isActive = true;
    res.status   = "BENCH";
    return res;
}


static CreateAllocationRequest validRequest() {
    CreateAllocationRequest req;
    req.userId      = 5;
    req.projectId   = 10;
    req.utilisation = 50;
    req.fromDate    = "2027-01-05";
    req.toDate      = "2027-06-30";
    return req;
}



class AllocationServiceTest : public ::testing::Test {
protected:
    std::shared_ptr<MockAllocationRepository> allocRepo;
    std::shared_ptr<MockResourceRepository>   resRepo;
    std::shared_ptr<MockProjectRepository>    projRepo;
    std::unique_ptr<AllocationService>        service;

    void SetUp() override {
        allocRepo = std::make_shared<NiceMock<MockAllocationRepository>>();
        resRepo   = std::make_shared<NiceMock<MockResourceRepository>>();
        projRepo  = std::make_shared<NiceMock<MockProjectRepository>>();
        service   = std::make_unique<AllocationService>(allocRepo, resRepo, projRepo);
    }
};



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
    req.fromDate = "not-allocation-date";
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



TEST_F(AllocationServiceTest, CreateAllocation_ProjectNotFound_ThrowsNotFound) {
    EXPECT_CALL(*projRepo, findById(10)).WillOnce(Return(std::nullopt));
    EXPECT_THROW(service->createAllocation(1, validRequest()), NotFoundException);
}

TEST_F(AllocationServiceTest, CreateAllocation_WrongManager_ThrowsUnauthorized) {
    auto proj = makeActiveProject(99);
    EXPECT_CALL(*projRepo, findById(10)).WillOnce(Return(proj));
    EXPECT_THROW(service->createAllocation(1, validRequest()), UnauthorizedException);
}

TEST_F(AllocationServiceTest, CreateAllocation_FromDateBeforeProjectStart_ThrowsValidation) {
    auto proj = makeActiveProject();
    proj.startDate = "2027-06-01";
    EXPECT_CALL(*projRepo, findById(10)).WillOnce(Return(proj));
    EXPECT_THROW(service->createAllocation(1, validRequest()), ValidationException);
}

TEST_F(AllocationServiceTest, CreateAllocation_ToDdateAfterProjectEnd_ThrowsValidation) {
    auto proj = makeActiveProject();
    proj.endDate = "2027-03-01";
    EXPECT_CALL(*projRepo, findById(10)).WillOnce(Return(proj));
    EXPECT_THROW(service->createAllocation(1, validRequest()), ValidationException);
}



TEST_F(AllocationServiceTest, CreateAllocation_ResourceNotFound_ThrowsNotFound) {
    EXPECT_CALL(*projRepo,  findById(10)).WillOnce(Return(makeActiveProject()));
    EXPECT_CALL(*resRepo,   findById(5)).WillOnce(Return(std::nullopt));
    EXPECT_THROW(service->createAllocation(1, validRequest()), NotFoundException);
}

TEST_F(AllocationServiceTest, CreateAllocation_InactiveResource_ThrowsValidation) {
    auto res = makeActiveResource();
    res.isActive = false;
    EXPECT_CALL(*projRepo, findById(10)).WillOnce(Return(makeActiveProject()));
    EXPECT_CALL(*resRepo,  findById(5)).WillOnce(Return(res));
    EXPECT_THROW(service->createAllocation(1, validRequest()), ValidationException);
}

TEST_F(AllocationServiceTest, CreateAllocation_WouldExceed100Percent_ThrowsValidation) {
    EXPECT_CALL(*projRepo,  findById(10)).WillOnce(Return(makeActiveProject()));
    EXPECT_CALL(*resRepo,   findById(5)).WillOnce(Return(makeActiveResource()));
    EXPECT_CALL(*allocRepo, getTotalActiveUtilisation(5)).WillOnce(Return(60));

    EXPECT_THROW(service->createAllocation(1, validRequest()), ValidationException);
}



TEST_F(AllocationServiceTest, CreateAllocation_ValidRequest_ReturnsAllocation) {
    EXPECT_CALL(*projRepo,  findById(10)).WillOnce(Return(makeActiveProject()));
    EXPECT_CALL(*resRepo,   findById(5)).WillOnce(Return(makeActiveResource()));
    EXPECT_CALL(*allocRepo, getTotalActiveUtilisation(5)).WillOnce(Return(30));
    EXPECT_CALL(*allocRepo, create(_)).WillOnce(Return(42));
    EXPECT_CALL(*resRepo,   setStatus(5, "ALLOCATED"));

    const Allocation result = service->createAllocation(1, validRequest());
    EXPECT_EQ(result.allocationId, 42);
    EXPECT_EQ(result.userId,       5);
    EXPECT_EQ(result.projectId,    10);
    EXPECT_EQ(result.utilisation,  50);
    EXPECT_TRUE(result.isActive);
}



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
    auto proj = makeActiveProject(99);
    EXPECT_CALL(*projRepo, findById(10)).WillOnce(Return(proj));
    EXPECT_THROW(service->endAllocation(1, 1), UnauthorizedException);
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
    EXPECT_CALL(*resRepo,   hasActiveAllocations(5)).WillOnce(Return(false));
    EXPECT_CALL(*resRepo,   setStatus(5, "BENCH"));

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
    EXPECT_CALL(*resRepo,   hasActiveAllocations(5)).WillOnce(Return(true));
    EXPECT_CALL(*resRepo,   setStatus(_, _)).Times(0);

    service->endAllocation(1, 1);
}



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
