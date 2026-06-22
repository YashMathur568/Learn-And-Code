#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "ResourceService.hpp"
#include "AppException.hpp"
#include "mocks/MockResourceRepository.hpp"
#include "mocks/MockAllocationRepository.hpp"
#include "mocks/MockSkillRepository.hpp"
#include "mocks/MockUserRepository.hpp"

using ::testing::Return;
using ::testing::_;
using ::testing::NiceMock;



static Resource makeResource(int userId = 1, bool active = true) {
    Resource res;
    res.userId   = userId;
    res.fullName = "Alice Smith";
    res.email    = "alice@example.com";
    res.isActive = active;
    res.status   = "BENCH";
    res.role     = "RESOURCE";
    return res;
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



class ResourceServiceTest : public ::testing::Test {
protected:
    std::shared_ptr<MockResourceRepository>   resRepo;
    std::shared_ptr<MockUserRepository>       userRepo;
    std::shared_ptr<MockSkillRepository>      skillRepo;
    std::shared_ptr<MockAllocationRepository> allocRepo;
    std::unique_ptr<ResourceService>          service;

    void SetUp() override {
        resRepo   = std::make_shared<NiceMock<MockResourceRepository>>();
        userRepo  = std::make_shared<NiceMock<MockUserRepository>>();
        skillRepo = std::make_shared<NiceMock<MockSkillRepository>>();
        allocRepo = std::make_shared<NiceMock<MockAllocationRepository>>();
        service   = std::make_unique<ResourceService>(resRepo, userRepo, skillRepo, allocRepo);
    }
};



TEST_F(ResourceServiceTest, GetById_Exists_ReturnsResource) {
    EXPECT_CALL(*resRepo, findById(1)).WillOnce(Return(makeResource(1)));
    const Resource result = service->getById(1);
    EXPECT_EQ(result.userId,   1);
    EXPECT_EQ(result.fullName, "Alice Smith");
}

TEST_F(ResourceServiceTest, GetById_NotFound_ThrowsNotFoundException) {
    EXPECT_CALL(*resRepo, findById(999)).WillOnce(Return(std::nullopt));
    EXPECT_THROW(service->getById(999), NotFoundException);
}



TEST_F(ResourceServiceTest, GetAllResources_DelegatesToRepository) {
    std::vector<Resource> resources = {makeResource(1), makeResource(2)};
    EXPECT_CALL(*resRepo, findAll()).WillOnce(Return(resources));
    EXPECT_EQ(service->getAllResources().size(), 2u);
}

TEST_F(ResourceServiceTest, GetByManagerId_DelegatesToRepository) {
    EXPECT_CALL(*resRepo, findByManagerId(5)).WillOnce(Return(std::vector<Resource>{}));
    EXPECT_TRUE(service->getByManagerId(5).empty());
}



TEST_F(ResourceServiceTest, UpdateResource_EmptyFullName_ThrowsValidation) {
    UpdateResourceRequest req;
    req.fullName = "";
    req.email    = "alice@example.com";
    EXPECT_THROW(service->updateResource(1, req), ValidationException);
}

TEST_F(ResourceServiceTest, UpdateResource_EmptyEmail_ThrowsValidation) {
    UpdateResourceRequest req;
    req.fullName = "Alice";
    req.email    = "";
    EXPECT_THROW(service->updateResource(1, req), ValidationException);
}

TEST_F(ResourceServiceTest, UpdateResource_ResourceNotFound_ThrowsNotFound) {
    UpdateResourceRequest req;
    req.fullName = "Alice";
    req.email    = "alice@example.com";
    EXPECT_CALL(*resRepo, findById(1)).WillOnce(Return(std::nullopt));
    EXPECT_THROW(service->updateResource(1, req), NotFoundException);
}

TEST_F(ResourceServiceTest, UpdateResource_ValidRequest_UpdatesAndReturns) {
    UpdateResourceRequest req;
    req.fullName    = "Alice Updated";
    req.email       = "alice.new@example.com";
    req.department  = "Engineering";
    req.designation = "Senior";

    EXPECT_CALL(*resRepo, findById(1)).WillOnce(Return(makeResource(1)));
    EXPECT_CALL(*resRepo, update(_));

    const Resource result = service->updateResource(1, req);
    EXPECT_EQ(result.fullName,    "Alice Updated");
    EXPECT_EQ(result.email,       "alice.new@example.com");
    EXPECT_EQ(result.department,  "Engineering");
    EXPECT_EQ(result.designation, "Senior");
}



TEST_F(ResourceServiceTest, DeactivateResource_NotFound_ThrowsNotFound) {
    EXPECT_CALL(*resRepo, findById(1)).WillOnce(Return(std::nullopt));
    EXPECT_THROW(service->deactivateResource(1), NotFoundException);
}

TEST_F(ResourceServiceTest, DeactivateResource_AlreadyInactive_ThrowsValidation) {
    EXPECT_CALL(*resRepo, findById(1)).WillOnce(Return(makeResource(1, false)));
    EXPECT_THROW(service->deactivateResource(1), ValidationException);
}

TEST_F(ResourceServiceTest, DeactivateResource_ActiveResource_EndsAllocationsAndDeactivates) {
    Allocation alloc;
    alloc.allocationId = 10;
    alloc.isActive     = true;
    std::vector<Allocation> activeAllocs = {alloc};

    EXPECT_CALL(*resRepo,   findById(1)).WillOnce(Return(makeResource(1)));
    EXPECT_CALL(*allocRepo, findActiveByUserId(1)).WillOnce(Return(activeAllocs));
    EXPECT_CALL(*allocRepo, endAllByUser(1));
    EXPECT_CALL(*resRepo,   setActiveStatus(1, false));
    EXPECT_CALL(*resRepo,   setStatus(1, "BENCH"));

    const auto ended = service->deactivateResource(1);
    EXPECT_EQ(ended.size(), 1u);
    EXPECT_EQ(ended[0].allocationId, 10);
}



TEST_F(ResourceServiceTest, AssignManager_ResourceNotFound_ThrowsNotFound) {
    EXPECT_CALL(*resRepo, findById(1)).WillOnce(Return(std::nullopt));
    AssignManagerRequest req;
    req.managerId = 99;
    EXPECT_THROW(service->assignManager(1, req), NotFoundException);
}

TEST_F(ResourceServiceTest, AssignManager_SelfAssignment_ThrowsValidation) {
    EXPECT_CALL(*resRepo, findById(1)).WillOnce(Return(makeResource(1)));
    AssignManagerRequest req;
    req.managerId = 1;
    EXPECT_THROW(service->assignManager(1, req), ValidationException);
}

TEST_F(ResourceServiceTest, AssignManager_UserNotManager_ThrowsValidation) {
    EXPECT_CALL(*resRepo, findById(1)).WillOnce(Return(makeResource(1)));
    User nonManager;
    nonManager.userId = 99;
    nonManager.role   = "RESOURCE";
    EXPECT_CALL(*userRepo, findById(99)).WillOnce(Return(nonManager));
    AssignManagerRequest req;
    req.managerId = 99;
    EXPECT_THROW(service->assignManager(1, req), ValidationException);
}

TEST_F(ResourceServiceTest, AssignManager_ManagerNotFound_ThrowsValidation) {
    EXPECT_CALL(*resRepo,  findById(1)).WillOnce(Return(makeResource(1)));
    EXPECT_CALL(*userRepo, findById(99)).WillOnce(Return(std::nullopt));
    AssignManagerRequest req;
    req.managerId = 99;
    EXPECT_THROW(service->assignManager(1, req), ValidationException);
}

TEST_F(ResourceServiceTest, AssignManager_ValidManager_CallsRepository) {
    EXPECT_CALL(*resRepo,  findById(1)).WillOnce(Return(makeResource(1)));
    EXPECT_CALL(*userRepo, findById(99)).WillOnce(Return(makeManagerUser(99)));
    EXPECT_CALL(*resRepo,  assignManager(1, 99));
    AssignManagerRequest req;
    req.managerId = 99;
    EXPECT_NO_THROW(service->assignManager(1, req));
}



TEST_F(ResourceServiceTest, GetSkills_ResourceNotFound_ThrowsNotFound) {
    EXPECT_CALL(*resRepo, findById(1)).WillOnce(Return(std::nullopt));
    EXPECT_THROW(service->getSkills(1), NotFoundException);
}

TEST_F(ResourceServiceTest, GetSkills_Exists_ReturnsSkills) {
    ResourceSkill skill;
    skill.skillId   = 1;
    skill.skillName = "Python";
    EXPECT_CALL(*resRepo,   findById(1)).WillOnce(Return(makeResource(1)));
    EXPECT_CALL(*skillRepo, findByUserId(1)).WillOnce(Return(std::vector<ResourceSkill>{skill}));
    EXPECT_EQ(service->getSkills(1).size(), 1u);
}



TEST_F(ResourceServiceTest, AddSkill_ResourceNotFound_ThrowsNotFound) {
    EXPECT_CALL(*resRepo, findById(1)).WillOnce(Return(std::nullopt));
    EXPECT_THROW(service->addSkill(1, validSkillRequest()), NotFoundException);
}

TEST_F(ResourceServiceTest, AddSkill_EmptySkillName_ThrowsValidation) {
    EXPECT_CALL(*resRepo, findById(1)).WillOnce(Return(makeResource(1)));
    auto req = validSkillRequest();
    req.skillName = "";
    EXPECT_THROW(service->addSkill(1, req), ValidationException);
}

TEST_F(ResourceServiceTest, AddSkill_InvalidCategory_ThrowsValidation) {
    EXPECT_CALL(*resRepo, findById(1)).WillOnce(Return(makeResource(1)));
    auto req = validSkillRequest();
    req.category = "InvalidCategory";
    EXPECT_THROW(service->addSkill(1, req), ValidationException);
}

TEST_F(ResourceServiceTest, AddSkill_InvalidProficiency_ThrowsValidation) {
    EXPECT_CALL(*resRepo, findById(1)).WillOnce(Return(makeResource(1)));
    auto req = validSkillRequest();
    req.proficiency = "Expert";
    EXPECT_THROW(service->addSkill(1, req), ValidationException);
}

TEST_F(ResourceServiceTest, AddSkill_DuplicateSkill_ThrowsValidation) {
    ResourceSkill existing;
    existing.skillId   = 5;
    existing.skillName = "Python";

    EXPECT_CALL(*resRepo,   findById(1)).WillOnce(Return(makeResource(1)));
    EXPECT_CALL(*skillRepo, findByUserId(1)).WillOnce(Return(std::vector<ResourceSkill>{existing}));
    EXPECT_THROW(service->addSkill(1, validSkillRequest("Python")), ValidationException);
}

TEST_F(ResourceServiceTest, AddSkill_ValidNewSkill_ReturnsSkill) {
    EXPECT_CALL(*resRepo,   findById(1)).WillOnce(Return(makeResource(1)));
    EXPECT_CALL(*skillRepo, findByUserId(1)).WillOnce(Return(std::vector<ResourceSkill>{}));
    EXPECT_CALL(*skillRepo, create(_)).WillOnce(Return(7));

    const ResourceSkill result = service->addSkill(1, validSkillRequest("Python"));
    EXPECT_EQ(result.skillId,     7);
    EXPECT_EQ(result.skillName,   "Python");
    EXPECT_EQ(result.category,    "Backend");
    EXPECT_EQ(result.proficiency, "Intermediate");
}



TEST_F(ResourceServiceTest, UpdateSkill_SkillNotBelongToUser_ThrowsNotFound) {
    EXPECT_CALL(*skillRepo, skillBelongsToUser(5, 1)).WillOnce(Return(false));
    EXPECT_THROW(service->updateSkill(1, 5, validSkillRequest()), NotFoundException);
}

TEST_F(ResourceServiceTest, UpdateSkill_InvalidCategory_ThrowsValidation) {
    EXPECT_CALL(*skillRepo, skillBelongsToUser(5, 1)).WillOnce(Return(true));
    auto req = validSkillRequest();
    req.category = "Nonsense";
    EXPECT_THROW(service->updateSkill(1, 5, req), ValidationException);
}

TEST_F(ResourceServiceTest, UpdateSkill_Valid_CallsRepositoryUpdate) {
    EXPECT_CALL(*skillRepo, skillBelongsToUser(5, 1)).WillOnce(Return(true));
    EXPECT_CALL(*skillRepo, update(_));
    EXPECT_NO_THROW(service->updateSkill(1, 5, validSkillRequest()));
}



TEST_F(ResourceServiceTest, RemoveSkill_SkillNotBelongToUser_ThrowsNotFound) {
    EXPECT_CALL(*skillRepo, skillBelongsToUser(5, 1)).WillOnce(Return(false));
    EXPECT_THROW(service->removeSkill(1, 5), NotFoundException);
}

TEST_F(ResourceServiceTest, RemoveSkill_SkillBelongsToUser_CallsRemove) {
    EXPECT_CALL(*skillRepo, skillBelongsToUser(5, 1)).WillOnce(Return(true));
    EXPECT_CALL(*skillRepo, remove(5));
    EXPECT_NO_THROW(service->removeSkill(1, 5));
}



TEST_F(ResourceServiceTest, UpdateResource_InvalidDepartment_ThrowsValidation) {
    UpdateResourceRequest req;
    req.fullName   = "Alice";
    req.email      = "alice@example.com";
    req.department = "InvalidDept";
    EXPECT_THROW(service->updateResource(1, req), ValidationException);
}

TEST_F(ResourceServiceTest, UpdateResource_InvalidDesignation_ThrowsValidation) {
    UpdateResourceRequest req;
    req.fullName    = "Alice";
    req.email       = "alice@example.com";
    req.designation = "GrandWizard";
    EXPECT_THROW(service->updateResource(1, req), ValidationException);
}



static ChangeRoleRequest makeChangeRoleRequest(
    const std::string& newRole,
    const std::string& department  = "",
    const std::string& designation = ""
) {
    ChangeRoleRequest req;
    req.newRole     = newRole;
    req.department  = department;
    req.designation = designation;
    return req;
}

static Resource makeResourceWithRole(int id, const std::string& role) {
    Resource res = makeResource(id);
    res.role = role;
    return res;
}

TEST_F(ResourceServiceTest, ChangeRole_InvalidRole_ThrowsValidation) {
    EXPECT_THROW(service->changeRole(1, makeChangeRoleRequest("SUPERUSER"), 99), ValidationException);
}

TEST_F(ResourceServiceTest, ChangeRole_SelfChange_ThrowsValidation) {
    EXPECT_THROW(service->changeRole(1, makeChangeRoleRequest("MANAGER", "", "Manager"), 1), ValidationException);
}

TEST_F(ResourceServiceTest, ChangeRole_ResourceNotFound_ThrowsNotFound) {
    EXPECT_CALL(*resRepo, findById(1)).WillOnce(Return(std::nullopt));
    EXPECT_THROW(service->changeRole(1, makeChangeRoleRequest("MANAGER", "", "Manager"), 99), NotFoundException);
}

TEST_F(ResourceServiceTest, ChangeRole_AlreadyHasRole_ThrowsValidation) {
    EXPECT_CALL(*resRepo, findById(1)).WillOnce(Return(makeResourceWithRole(1, "RESOURCE")));
    EXPECT_THROW(service->changeRole(1, makeChangeRoleRequest("RESOURCE"), 99), ValidationException);
}

TEST_F(ResourceServiceTest, ChangeRole_LastAdmin_ThrowsValidation) {
    EXPECT_CALL(*resRepo,  findById(1)).WillOnce(Return(makeResourceWithRole(1, "ADMIN")));
    EXPECT_CALL(*userRepo, countActiveAdmins()).WillOnce(Return(1));
    EXPECT_THROW(service->changeRole(1, makeChangeRoleRequest("RESOURCE"), 99), ValidationException);
}

TEST_F(ResourceServiceTest, ChangeRole_ManagerWithActiveProjects_ThrowsValidation) {
    EXPECT_CALL(*resRepo, findById(1)).WillOnce(Return(makeResourceWithRole(1, "MANAGER")));
    EXPECT_CALL(*resRepo, countManagedProjects(1)).WillOnce(Return(2));
    EXPECT_THROW(service->changeRole(1, makeChangeRoleRequest("RESOURCE"), 99), ValidationException);
}

TEST_F(ResourceServiceTest, ChangeRole_ToManager_MissingDesignation_ThrowsValidation) {
    EXPECT_CALL(*resRepo, findById(1)).WillOnce(Return(makeResourceWithRole(1, "RESOURCE")));
    EXPECT_CALL(*allocRepo, endAllByUser(1));
    EXPECT_CALL(*resRepo,   deleteResourceStatus(1));
    EXPECT_THROW(service->changeRole(1, makeChangeRoleRequest("MANAGER"), 99), ValidationException);
}

TEST_F(ResourceServiceTest, ChangeRole_ToManager_InvalidDesignation_ThrowsValidation) {
    EXPECT_CALL(*resRepo, findById(1)).WillOnce(Return(makeResourceWithRole(1, "RESOURCE")));
    EXPECT_CALL(*allocRepo, endAllByUser(1));
    EXPECT_CALL(*resRepo,   deleteResourceStatus(1));
    EXPECT_THROW(service->changeRole(1, makeChangeRoleRequest("MANAGER", "", "BadDesig"), 99), ValidationException);
}

TEST_F(ResourceServiceTest, ChangeRole_ToManager_Success) {
    Resource updated = makeResourceWithRole(1, "MANAGER");
    EXPECT_CALL(*resRepo,  findById(1))
        .WillOnce(Return(makeResourceWithRole(1, "RESOURCE")))
        .WillOnce(Return(updated));
    EXPECT_CALL(*allocRepo, endAllByUser(1));
    EXPECT_CALL(*resRepo,   deleteResourceStatus(1));
    EXPECT_CALL(*resRepo,   update(_));
    EXPECT_CALL(*userRepo,  updateRole(1, "MANAGER"));

    Resource result = service->changeRole(1, makeChangeRoleRequest("MANAGER", "", "Manager"), 99);
    EXPECT_EQ(result.role, "MANAGER");
}

TEST_F(ResourceServiceTest, ChangeRole_ToAdmin_Success) {
    Resource updated = makeResourceWithRole(1, "ADMIN");
    EXPECT_CALL(*resRepo,  findById(1))
        .WillOnce(Return(makeResourceWithRole(1, "RESOURCE")))
        .WillOnce(Return(updated));
    EXPECT_CALL(*allocRepo, endAllByUser(1));
    EXPECT_CALL(*resRepo,   deleteResourceStatus(1));
    EXPECT_CALL(*resRepo,   update(_));
    EXPECT_CALL(*userRepo,  updateRole(1, "ADMIN"));

    Resource result = service->changeRole(1, makeChangeRoleRequest("ADMIN"), 99);
    EXPECT_EQ(result.role, "ADMIN");
}

TEST_F(ResourceServiceTest, ChangeRole_ToResource_MissingDepartment_ThrowsValidation) {
    EXPECT_CALL(*resRepo,  findById(1)).WillOnce(Return(makeResourceWithRole(1, "MANAGER")));
    EXPECT_CALL(*resRepo,  countManagedProjects(1)).WillOnce(Return(0));
    EXPECT_CALL(*resRepo,  createResourceStatus(1));
    EXPECT_THROW(service->changeRole(1, makeChangeRoleRequest("RESOURCE", "", "Junior"), 99), ValidationException);
}

TEST_F(ResourceServiceTest, ChangeRole_ToResource_InvalidDepartment_ThrowsValidation) {
    EXPECT_CALL(*resRepo,  findById(1)).WillOnce(Return(makeResourceWithRole(1, "MANAGER")));
    EXPECT_CALL(*resRepo,  countManagedProjects(1)).WillOnce(Return(0));
    EXPECT_CALL(*resRepo,  createResourceStatus(1));
    EXPECT_THROW(service->changeRole(1, makeChangeRoleRequest("RESOURCE", "BadDept", "Junior"), 99), ValidationException);
}

TEST_F(ResourceServiceTest, ChangeRole_ToResource_InvalidDesignation_ThrowsValidation) {
    EXPECT_CALL(*resRepo,  findById(1)).WillOnce(Return(makeResourceWithRole(1, "MANAGER")));
    EXPECT_CALL(*resRepo,  countManagedProjects(1)).WillOnce(Return(0));
    EXPECT_CALL(*resRepo,  createResourceStatus(1));
    EXPECT_THROW(service->changeRole(1, makeChangeRoleRequest("RESOURCE", "Engineering", "BadDesig"), 99), ValidationException);
}

TEST_F(ResourceServiceTest, ChangeRole_ToResource_Success) {
    Resource updated = makeResourceWithRole(1, "RESOURCE");
    EXPECT_CALL(*resRepo,  findById(1))
        .WillOnce(Return(makeResourceWithRole(1, "MANAGER")))
        .WillOnce(Return(updated));
    EXPECT_CALL(*resRepo,  countManagedProjects(1)).WillOnce(Return(0));
    EXPECT_CALL(*resRepo,  createResourceStatus(1));
    EXPECT_CALL(*resRepo,  update(_));
    EXPECT_CALL(*userRepo, updateRole(1, "RESOURCE"));

    Resource result = service->changeRole(1, makeChangeRoleRequest("RESOURCE", "Engineering", "Junior"), 99);
    EXPECT_EQ(result.role, "RESOURCE");
}

TEST_F(ResourceServiceTest, ChangeRole_AdminToResource_MultipleAdmins_Success) {
    Resource updated = makeResourceWithRole(1, "RESOURCE");
    EXPECT_CALL(*resRepo,  findById(1))
        .WillOnce(Return(makeResourceWithRole(1, "ADMIN")))
        .WillOnce(Return(updated));
    EXPECT_CALL(*userRepo, countActiveAdmins()).WillOnce(Return(3));
    EXPECT_CALL(*resRepo,  createResourceStatus(1));
    EXPECT_CALL(*resRepo,  update(_));
    EXPECT_CALL(*userRepo, updateRole(1, "RESOURCE"));

    Resource result = service->changeRole(1, makeChangeRoleRequest("RESOURCE", "Engineering", "Junior"), 99);
    EXPECT_EQ(result.role, "RESOURCE");
}
