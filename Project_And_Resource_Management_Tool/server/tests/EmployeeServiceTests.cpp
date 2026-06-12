#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "services/EmployeeService.hpp"
#include "utils/AppException.hpp"
#include "mocks/MockEmployeeRepository.hpp"
#include "mocks/MockAllocationRepository.hpp"
#include "mocks/MockSkillRepository.hpp"
#include "mocks/MockUserRepository.hpp"

using ::testing::Return;
using ::testing::_;
using ::testing::NiceMock;

// ── Helpers ───────────────────────────────────────────────────────────────────

static Employee makeEmployee(int userId = 1, bool active = true) {
    Employee emp;
    emp.userId   = userId;
    emp.fullName = "Alice Smith";
    emp.email    = "alice@example.com";
    emp.isActive = active;
    emp.status   = "BENCH";
    emp.role     = "RESOURCE";
    return emp;
}

static User makeManagerUser(int userId = 99) {
    User user;
    user.userId = userId;
    user.role   = "MANAGER";
    user.isActive = true;
    return user;
}

static SkillRequest validSkillRequest(const std::string& name = "Python") {
    SkillRequest req;
    req.skillName   = name;
    req.category    = "Backend";
    req.proficiency = "Intermediate";
    return req;
}

// ── Fixture ───────────────────────────────────────────────────────────────────

class EmployeeServiceTest : public ::testing::Test {
protected:
    std::shared_ptr<MockEmployeeRepository>   empRepo;
    std::shared_ptr<MockUserRepository>       userRepo;
    std::shared_ptr<MockSkillRepository>      skillRepo;
    std::shared_ptr<MockAllocationRepository> allocRepo;
    std::unique_ptr<EmployeeService>          service;

    void SetUp() override {
        empRepo   = std::make_shared<NiceMock<MockEmployeeRepository>>();
        userRepo  = std::make_shared<NiceMock<MockUserRepository>>();
        skillRepo = std::make_shared<NiceMock<MockSkillRepository>>();
        allocRepo = std::make_shared<NiceMock<MockAllocationRepository>>();
        service   = std::make_unique<EmployeeService>(empRepo, userRepo, skillRepo, allocRepo);
    }
};

// ── getById ───────────────────────────────────────────────────────────────────

TEST_F(EmployeeServiceTest, GetById_Exists_ReturnsEmployee) {
    EXPECT_CALL(*empRepo, findById(1)).WillOnce(Return(makeEmployee(1)));
    const Employee result = service->getById(1);
    EXPECT_EQ(result.userId,   1);
    EXPECT_EQ(result.fullName, "Alice Smith");
}

TEST_F(EmployeeServiceTest, GetById_NotFound_ThrowsNotFoundException) {
    EXPECT_CALL(*empRepo, findById(999)).WillOnce(Return(std::nullopt));
    EXPECT_THROW(service->getById(999), NotFoundException);
}

// ── getAllEmployees / getByManagerId ──────────────────────────────────────────

TEST_F(EmployeeServiceTest, GetAllEmployees_DelegatesToRepository) {
    std::vector<Employee> employees = {makeEmployee(1), makeEmployee(2)};
    EXPECT_CALL(*empRepo, findAll()).WillOnce(Return(employees));
    EXPECT_EQ(service->getAllEmployees().size(), 2u);
}

TEST_F(EmployeeServiceTest, GetByManagerId_DelegatesToRepository) {
    EXPECT_CALL(*empRepo, findByManagerId(5)).WillOnce(Return(std::vector<Employee>{}));
    EXPECT_TRUE(service->getByManagerId(5).empty());
}

// ── updateEmployee ────────────────────────────────────────────────────────────

TEST_F(EmployeeServiceTest, UpdateEmployee_EmptyFullName_ThrowsValidation) {
    UpdateEmployeeRequest req;
    req.fullName = "";
    req.email    = "alice@example.com";
    EXPECT_THROW(service->updateEmployee(1, req), ValidationException);
}

TEST_F(EmployeeServiceTest, UpdateEmployee_EmptyEmail_ThrowsValidation) {
    UpdateEmployeeRequest req;
    req.fullName = "Alice";
    req.email    = "";
    EXPECT_THROW(service->updateEmployee(1, req), ValidationException);
}

TEST_F(EmployeeServiceTest, UpdateEmployee_EmployeeNotFound_ThrowsNotFound) {
    UpdateEmployeeRequest req;
    req.fullName = "Alice";
    req.email    = "alice@example.com";
    EXPECT_CALL(*empRepo, findById(1)).WillOnce(Return(std::nullopt));
    EXPECT_THROW(service->updateEmployee(1, req), NotFoundException);
}

TEST_F(EmployeeServiceTest, UpdateEmployee_ValidRequest_UpdatesAndReturns) {
    UpdateEmployeeRequest req;
    req.fullName    = "Alice Updated";
    req.email       = "alice.new@example.com";
    req.department  = "Engineering";
    req.designation = "Senior Dev";

    EXPECT_CALL(*empRepo, findById(1)).WillOnce(Return(makeEmployee(1)));
    EXPECT_CALL(*empRepo, update(_));

    const Employee result = service->updateEmployee(1, req);
    EXPECT_EQ(result.fullName,    "Alice Updated");
    EXPECT_EQ(result.email,       "alice.new@example.com");
    EXPECT_EQ(result.department,  "Engineering");
    EXPECT_EQ(result.designation, "Senior Dev");
}

// ── deactivateEmployee ────────────────────────────────────────────────────────

TEST_F(EmployeeServiceTest, DeactivateEmployee_NotFound_ThrowsNotFound) {
    EXPECT_CALL(*empRepo, findById(1)).WillOnce(Return(std::nullopt));
    EXPECT_THROW(service->deactivateEmployee(1), NotFoundException);
}

TEST_F(EmployeeServiceTest, DeactivateEmployee_AlreadyInactive_ThrowsValidation) {
    EXPECT_CALL(*empRepo, findById(1)).WillOnce(Return(makeEmployee(1, /*active=*/false)));
    EXPECT_THROW(service->deactivateEmployee(1), ValidationException);
}

TEST_F(EmployeeServiceTest, DeactivateEmployee_ActiveEmployee_EndsAllocationsAndDeactivates) {
    Allocation alloc;
    alloc.allocationId = 10;
    alloc.isActive     = true;
    std::vector<Allocation> activeAllocs = {alloc};

    EXPECT_CALL(*empRepo,   findById(1)).WillOnce(Return(makeEmployee(1)));
    EXPECT_CALL(*allocRepo, findActiveByUserId(1)).WillOnce(Return(activeAllocs));
    EXPECT_CALL(*allocRepo, endAllByUser(1));
    EXPECT_CALL(*empRepo,   setActiveStatus(1, false));
    EXPECT_CALL(*empRepo,   setStatus(1, "BENCH"));

    const auto ended = service->deactivateEmployee(1);
    EXPECT_EQ(ended.size(), 1u);
    EXPECT_EQ(ended[0].allocationId, 10);
}

// ── assignManager ─────────────────────────────────────────────────────────────

TEST_F(EmployeeServiceTest, AssignManager_EmployeeNotFound_ThrowsNotFound) {
    EXPECT_CALL(*empRepo, findById(1)).WillOnce(Return(std::nullopt));
    AssignManagerRequest req;
    req.managerId = 99;
    EXPECT_THROW(service->assignManager(1, req), NotFoundException);
}

TEST_F(EmployeeServiceTest, AssignManager_SelfAssignment_ThrowsValidation) {
    EXPECT_CALL(*empRepo, findById(1)).WillOnce(Return(makeEmployee(1)));
    AssignManagerRequest req;
    req.managerId = 1;  // same as employeeId
    EXPECT_THROW(service->assignManager(1, req), ValidationException);
}

TEST_F(EmployeeServiceTest, AssignManager_UserNotManager_ThrowsValidation) {
    EXPECT_CALL(*empRepo, findById(1)).WillOnce(Return(makeEmployee(1)));
    User nonManager;
    nonManager.userId = 99;
    nonManager.role   = "RESOURCE";
    EXPECT_CALL(*userRepo, findById(99)).WillOnce(Return(nonManager));
    AssignManagerRequest req;
    req.managerId = 99;
    EXPECT_THROW(service->assignManager(1, req), ValidationException);
}

TEST_F(EmployeeServiceTest, AssignManager_ManagerNotFound_ThrowsValidation) {
    EXPECT_CALL(*empRepo,  findById(1)).WillOnce(Return(makeEmployee(1)));
    EXPECT_CALL(*userRepo, findById(99)).WillOnce(Return(std::nullopt));
    AssignManagerRequest req;
    req.managerId = 99;
    EXPECT_THROW(service->assignManager(1, req), ValidationException);
}

TEST_F(EmployeeServiceTest, AssignManager_ValidManager_CallsRepository) {
    EXPECT_CALL(*empRepo,  findById(1)).WillOnce(Return(makeEmployee(1)));
    EXPECT_CALL(*userRepo, findById(99)).WillOnce(Return(makeManagerUser(99)));
    EXPECT_CALL(*empRepo,  assignManager(1, 99));
    AssignManagerRequest req;
    req.managerId = 99;
    EXPECT_NO_THROW(service->assignManager(1, req));
}

// ── getSkills ─────────────────────────────────────────────────────────────────

TEST_F(EmployeeServiceTest, GetSkills_EmployeeNotFound_ThrowsNotFound) {
    EXPECT_CALL(*empRepo, findById(1)).WillOnce(Return(std::nullopt));
    EXPECT_THROW(service->getSkills(1), NotFoundException);
}

TEST_F(EmployeeServiceTest, GetSkills_Exists_ReturnsSkills) {
    EmployeeSkill skill;
    skill.skillId   = 1;
    skill.skillName = "Python";
    EXPECT_CALL(*empRepo,   findById(1)).WillOnce(Return(makeEmployee(1)));
    EXPECT_CALL(*skillRepo, findByUserId(1)).WillOnce(Return(std::vector<EmployeeSkill>{skill}));
    EXPECT_EQ(service->getSkills(1).size(), 1u);
}

// ── addSkill ──────────────────────────────────────────────────────────────────

TEST_F(EmployeeServiceTest, AddSkill_EmployeeNotFound_ThrowsNotFound) {
    EXPECT_CALL(*empRepo, findById(1)).WillOnce(Return(std::nullopt));
    EXPECT_THROW(service->addSkill(1, validSkillRequest()), NotFoundException);
}

TEST_F(EmployeeServiceTest, AddSkill_EmptySkillName_ThrowsValidation) {
    EXPECT_CALL(*empRepo, findById(1)).WillOnce(Return(makeEmployee(1)));
    auto req = validSkillRequest();
    req.skillName = "";
    EXPECT_THROW(service->addSkill(1, req), ValidationException);
}

TEST_F(EmployeeServiceTest, AddSkill_InvalidCategory_ThrowsValidation) {
    EXPECT_CALL(*empRepo, findById(1)).WillOnce(Return(makeEmployee(1)));
    auto req = validSkillRequest();
    req.category = "InvalidCategory";
    EXPECT_THROW(service->addSkill(1, req), ValidationException);
}

TEST_F(EmployeeServiceTest, AddSkill_InvalidProficiency_ThrowsValidation) {
    EXPECT_CALL(*empRepo, findById(1)).WillOnce(Return(makeEmployee(1)));
    auto req = validSkillRequest();
    req.proficiency = "Expert";  // not in valid list
    EXPECT_THROW(service->addSkill(1, req), ValidationException);
}

TEST_F(EmployeeServiceTest, AddSkill_DuplicateSkill_ThrowsValidation) {
    EmployeeSkill existing;
    existing.skillId   = 5;
    existing.skillName = "Python";

    EXPECT_CALL(*empRepo,   findById(1)).WillOnce(Return(makeEmployee(1)));
    EXPECT_CALL(*skillRepo, findByUserId(1)).WillOnce(Return(std::vector<EmployeeSkill>{existing}));
    EXPECT_THROW(service->addSkill(1, validSkillRequest("Python")), ValidationException);
}

TEST_F(EmployeeServiceTest, AddSkill_ValidNewSkill_ReturnsSkill) {
    EXPECT_CALL(*empRepo,   findById(1)).WillOnce(Return(makeEmployee(1)));
    EXPECT_CALL(*skillRepo, findByUserId(1)).WillOnce(Return(std::vector<EmployeeSkill>{}));
    EXPECT_CALL(*skillRepo, create(_)).WillOnce(Return(7));

    const EmployeeSkill result = service->addSkill(1, validSkillRequest("Python"));
    EXPECT_EQ(result.skillId,     7);
    EXPECT_EQ(result.skillName,   "Python");
    EXPECT_EQ(result.category,    "Backend");
    EXPECT_EQ(result.proficiency, "Intermediate");
}

// ── updateSkill ───────────────────────────────────────────────────────────────

TEST_F(EmployeeServiceTest, UpdateSkill_SkillNotBelongToUser_ThrowsNotFound) {
    EXPECT_CALL(*skillRepo, skillBelongsToUser(5, 1)).WillOnce(Return(false));
    EXPECT_THROW(service->updateSkill(1, 5, validSkillRequest()), NotFoundException);
}

TEST_F(EmployeeServiceTest, UpdateSkill_InvalidCategory_ThrowsValidation) {
    EXPECT_CALL(*skillRepo, skillBelongsToUser(5, 1)).WillOnce(Return(true));
    auto req = validSkillRequest();
    req.category = "Nonsense";
    EXPECT_THROW(service->updateSkill(1, 5, req), ValidationException);
}

TEST_F(EmployeeServiceTest, UpdateSkill_Valid_CallsRepositoryUpdate) {
    EXPECT_CALL(*skillRepo, skillBelongsToUser(5, 1)).WillOnce(Return(true));
    EXPECT_CALL(*skillRepo, update(_));
    EXPECT_NO_THROW(service->updateSkill(1, 5, validSkillRequest()));
}

// ── removeSkill ───────────────────────────────────────────────────────────────

TEST_F(EmployeeServiceTest, RemoveSkill_SkillNotBelongToUser_ThrowsNotFound) {
    EXPECT_CALL(*skillRepo, skillBelongsToUser(5, 1)).WillOnce(Return(false));
    EXPECT_THROW(service->removeSkill(1, 5), NotFoundException);
}

TEST_F(EmployeeServiceTest, RemoveSkill_SkillBelongsToUser_CallsRemove) {
    EXPECT_CALL(*skillRepo, skillBelongsToUser(5, 1)).WillOnce(Return(true));
    EXPECT_CALL(*skillRepo, remove(5));
    EXPECT_NO_THROW(service->removeSkill(1, 5));
}
