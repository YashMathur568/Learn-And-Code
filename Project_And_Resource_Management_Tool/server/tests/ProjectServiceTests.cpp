#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "ProjectService.hpp"
#include "AppException.hpp"
#include "mocks/MockProjectRepository.hpp"
#include "mocks/MockMilestoneRepository.hpp"
#include "mocks/MockResourceRepository.hpp"

using ::testing::Return;
using ::testing::_;
using ::testing::NiceMock;


static Project makeProject(int id = 10) {
    Project proj;
    proj.projectId = id;
    proj.name      = "Test Project";
    proj.startDate = "2025-01-01";
    proj.endDate   = "2030-12-31";
    proj.status    = "ACTIVE";
    proj.managerId = 1;
    return proj;
}

static Resource makeManager(int id = 1) {
    Resource res;
    res.userId   = id;
    res.role     = "MANAGER";
    res.fullName = "Jane Manager";
    res.isActive = true;
    return res;
}

static CreateProjectRequest validCreateRequest() {
    CreateProjectRequest req;
    req.name      = "New Project";
    req.startDate = "2025-01-01";
    req.endDate   = "2030-12-31";
    req.status    = "PLANNED";
    req.managerId = 1;
    return req;
}

class ProjectServiceTest : public ::testing::Test {
protected:
    std::shared_ptr<MockProjectRepository>   projRepo;
    std::shared_ptr<MockMilestoneRepository> msRepo;
    std::shared_ptr<MockResourceRepository>  resRepo;
    std::unique_ptr<ProjectService>          service;

    void SetUp() override {
        projRepo = std::make_shared<NiceMock<MockProjectRepository>>();
        msRepo   = std::make_shared<NiceMock<MockMilestoneRepository>>();
        resRepo  = std::make_shared<NiceMock<MockResourceRepository>>();
        service  = std::make_unique<ProjectService>(projRepo, msRepo, resRepo);
    }
};


// ── createProject ─────────────────────────────────────────────────────────────

TEST_F(ProjectServiceTest, CreateProject_EmptyName_ThrowsValidation) {
    CreateProjectRequest req = validCreateRequest();
    req.name = "";
    EXPECT_THROW(service->createProject(req), ValidationException);
}

TEST_F(ProjectServiceTest, CreateProject_InvalidStatus_ThrowsValidation) {
    CreateProjectRequest req = validCreateRequest();
    req.status = "FINISHED";
    EXPECT_THROW(service->createProject(req), ValidationException);
}

TEST_F(ProjectServiceTest, CreateProject_ManagerNotFound_ThrowsNotFound) {
    EXPECT_CALL(*resRepo, findById(1)).WillOnce(Return(std::nullopt));
    EXPECT_THROW(service->createProject(validCreateRequest()), NotFoundException);
}

TEST_F(ProjectServiceTest, CreateProject_UserIsNotManager_ThrowsValidation) {
    Resource nonManager = makeManager();
    nonManager.role = "RESOURCE";
    EXPECT_CALL(*resRepo, findById(1)).WillOnce(Return(nonManager));
    EXPECT_THROW(service->createProject(validCreateRequest()), ValidationException);
}

TEST_F(ProjectServiceTest, CreateProject_StartAfterEnd_ThrowsValidation) {
    CreateProjectRequest req = validCreateRequest();
    req.startDate = "2030-12-31";
    req.endDate   = "2025-01-01";
    EXPECT_CALL(*resRepo, findById(1)).WillOnce(Return(makeManager()));
    EXPECT_THROW(service->createProject(req), ValidationException);
}

TEST_F(ProjectServiceTest, CreateProject_InvalidStartDate_ThrowsValidation) {
    CreateProjectRequest req = validCreateRequest();
    req.startDate = "not-a-date";
    EXPECT_CALL(*resRepo, findById(1)).WillOnce(Return(makeManager()));
    EXPECT_THROW(service->createProject(req), ValidationException);
}

TEST_F(ProjectServiceTest, CreateProject_Success) {
    Project created = makeProject(20);
    EXPECT_CALL(*resRepo, findById(1)).WillOnce(Return(makeManager()));
    EXPECT_CALL(*projRepo, create(_)).WillOnce(Return(20));
    EXPECT_CALL(*projRepo, findById(20)).WillOnce(Return(created));

    Project result = service->createProject(validCreateRequest());
    EXPECT_EQ(result.projectId, 20);
}


// ── getProjectById ────────────────────────────────────────────────────────────

TEST_F(ProjectServiceTest, GetProjectById_NotFound_ThrowsNotFound) {
    EXPECT_CALL(*projRepo, findById(99)).WillOnce(Return(std::nullopt));
    EXPECT_THROW(service->getProjectById(99), NotFoundException);
}

TEST_F(ProjectServiceTest, GetProjectById_Found_ReturnsProject) {
    EXPECT_CALL(*projRepo, findById(10)).WillOnce(Return(makeProject(10)));
    Project result = service->getProjectById(10);
    EXPECT_EQ(result.projectId, 10);
}


// ── updateProject ─────────────────────────────────────────────────────────────

TEST_F(ProjectServiceTest, UpdateProject_NotFound_ThrowsNotFound) {
    EXPECT_CALL(*projRepo, findById(99)).WillOnce(Return(std::nullopt));
    UpdateProjectRequest req;
    EXPECT_THROW(service->updateProject(99, req), NotFoundException);
}

TEST_F(ProjectServiceTest, UpdateProject_InvalidStatus_ThrowsValidation) {
    EXPECT_CALL(*projRepo, findById(10)).WillOnce(Return(makeProject(10)));
    UpdateProjectRequest req;
    req.status = "INVALID";
    EXPECT_THROW(service->updateProject(10, req), ValidationException);
}

TEST_F(ProjectServiceTest, UpdateProject_StartEqualsEnd_ThrowsValidation) {
    EXPECT_CALL(*projRepo, findById(10)).WillOnce(Return(makeProject(10)));
    UpdateProjectRequest req;
    req.endDate = "2025-01-01";  // same as existing startDate
    EXPECT_THROW(service->updateProject(10, req), ValidationException);
}

TEST_F(ProjectServiceTest, UpdateProject_Success) {
    Project existing = makeProject(10);
    Project updated  = existing;
    updated.name = "Renamed";
    EXPECT_CALL(*projRepo, findById(10))
        .WillOnce(Return(existing))
        .WillOnce(Return(updated));
    EXPECT_CALL(*projRepo, update(_)).Times(1);

    UpdateProjectRequest req;
    req.name = "Renamed";
    Project result = service->updateProject(10, req);
    EXPECT_EQ(result.name, "Renamed");
}


// ── createMilestone ───────────────────────────────────────────────────────────

TEST_F(ProjectServiceTest, CreateMilestone_ProjectNotFound_ThrowsNotFound) {
    EXPECT_CALL(*projRepo, findById(99)).WillOnce(Return(std::nullopt));
    CreateMilestoneRequest req;
    EXPECT_THROW(service->createMilestone(99, req), NotFoundException);
}

TEST_F(ProjectServiceTest, CreateMilestone_EmptyTitle_ThrowsValidation) {
    EXPECT_CALL(*projRepo, findById(10)).WillOnce(Return(makeProject(10)));
    CreateMilestoneRequest req;
    req.title   = "";
    req.dueDate = "2026-06-01";
    req.status  = "NOT_STARTED";
    EXPECT_THROW(service->createMilestone(10, req), ValidationException);
}

TEST_F(ProjectServiceTest, CreateMilestone_InvalidStatus_ThrowsValidation) {
    EXPECT_CALL(*projRepo, findById(10)).WillOnce(Return(makeProject(10)));
    CreateMilestoneRequest req;
    req.title   = "Phase 1";
    req.dueDate = "2026-06-01";
    req.status  = "COMPLETED";
    EXPECT_THROW(service->createMilestone(10, req), ValidationException);
}

TEST_F(ProjectServiceTest, CreateMilestone_DueDateBeforeProjectStart_ThrowsValidation) {
    EXPECT_CALL(*projRepo, findById(10)).WillOnce(Return(makeProject(10)));
    CreateMilestoneRequest req;
    req.title   = "Phase 1";
    req.dueDate = "2020-01-01";  // before project startDate 2025-01-01
    req.status  = "NOT_STARTED";
    EXPECT_THROW(service->createMilestone(10, req), ValidationException);
}

TEST_F(ProjectServiceTest, CreateMilestone_Success) {
    Milestone ms;
    ms.milestoneId = 5;
    ms.projectId   = 10;
    ms.title       = "Phase 1";
    EXPECT_CALL(*projRepo, findById(10)).WillOnce(Return(makeProject(10)));
    EXPECT_CALL(*msRepo, create(_)).WillOnce(Return(5));
    EXPECT_CALL(*msRepo, findByProjectId(10)).WillOnce(Return(std::vector<Milestone>{ms}));

    CreateMilestoneRequest req;
    req.title   = "Phase 1";
    req.dueDate = "2026-06-01";
    req.status  = "NOT_STARTED";
    auto result = service->createMilestone(10, req);
    EXPECT_EQ(result.size(), 1u);
}


// ── getMilestones ─────────────────────────────────────────────────────────────

TEST_F(ProjectServiceTest, GetMilestones_ProjectNotFound_ThrowsNotFound) {
    EXPECT_CALL(*projRepo, findById(99)).WillOnce(Return(std::nullopt));
    EXPECT_THROW(service->getMilestones(99), NotFoundException);
}

TEST_F(ProjectServiceTest, GetMilestones_ReturnsAll) {
    Milestone ms;
    ms.milestoneId = 1;
    ms.projectId   = 10;
    EXPECT_CALL(*projRepo, findById(10)).WillOnce(Return(makeProject(10)));
    EXPECT_CALL(*msRepo, findByProjectId(10)).WillOnce(Return(std::vector<Milestone>{ms}));
    auto result = service->getMilestones(10);
    EXPECT_EQ(result.size(), 1u);
}


// ── updateMilestone ───────────────────────────────────────────────────────────

TEST_F(ProjectServiceTest, UpdateMilestone_MilestoneNotInProject_ThrowsNotFound) {
    EXPECT_CALL(*msRepo, milestoneBelongsToProject(5, 10)).WillOnce(Return(false));
    UpdateMilestoneRequest req;
    EXPECT_THROW(service->updateMilestone(10, 5, req), NotFoundException);
}

TEST_F(ProjectServiceTest, UpdateMilestone_MilestoneNotFound_ThrowsNotFound) {
    EXPECT_CALL(*msRepo, milestoneBelongsToProject(5, 10)).WillOnce(Return(true));
    EXPECT_CALL(*msRepo, findById(5)).WillOnce(Return(std::nullopt));
    UpdateMilestoneRequest req;
    EXPECT_THROW(service->updateMilestone(10, 5, req), NotFoundException);
}

TEST_F(ProjectServiceTest, UpdateMilestone_InvalidStatus_ThrowsValidation) {
    Milestone ms;
    ms.milestoneId = 5;
    ms.projectId   = 10;
    ms.title       = "Phase 1";
    ms.dueDate     = "2026-06-01";
    ms.status      = "NOT_STARTED";
    EXPECT_CALL(*msRepo, milestoneBelongsToProject(5, 10)).WillOnce(Return(true));
    EXPECT_CALL(*msRepo, findById(5)).WillOnce(Return(ms));
    UpdateMilestoneRequest req;
    req.status = "INVALID";
    EXPECT_THROW(service->updateMilestone(10, 5, req), ValidationException);
}

TEST_F(ProjectServiceTest, UpdateMilestone_Success) {
    Milestone existing;
    existing.milestoneId = 5;
    existing.projectId   = 10;
    existing.title       = "Phase 1";
    existing.dueDate     = "2026-06-01";
    existing.status      = "NOT_STARTED";

    Milestone updated = existing;
    updated.status = "IN_PROGRESS";

    EXPECT_CALL(*msRepo, milestoneBelongsToProject(5, 10)).WillOnce(Return(true));
    EXPECT_CALL(*msRepo, findById(5)).WillOnce(Return(existing));
    EXPECT_CALL(*msRepo, update(_)).Times(1);
    EXPECT_CALL(*msRepo, findByProjectId(10)).WillOnce(Return(std::vector<Milestone>{updated}));

    UpdateMilestoneRequest req;
    req.status = "IN_PROGRESS";
    auto result = service->updateMilestone(10, 5, req);
    EXPECT_EQ(result.front().status, "IN_PROGRESS");
}
