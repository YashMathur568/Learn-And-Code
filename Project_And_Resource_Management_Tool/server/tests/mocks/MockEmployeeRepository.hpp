#pragma once
#include "../../src/repositories/IEmployeeRepository.hpp"
#include <gmock/gmock.h>

class MockEmployeeRepository : public IEmployeeRepository {
public:
    MOCK_METHOD(std::optional<Employee>, findById,          (int userId), (override));
    MOCK_METHOD(std::vector<Employee>,   findAll,           (), (override));
    MOCK_METHOD(std::vector<Employee>,   findAllActive,     (), (override));
    MOCK_METHOD(std::vector<Employee>,   findByManagerId,   (int managerUserId), (override));
    MOCK_METHOD(int,                     create,            (const Employee& employee), (override));
    MOCK_METHOD(void,                    update,            (const Employee& employee), (override));
    MOCK_METHOD(void,                    setActiveStatus,   (int userId, bool active), (override));
    MOCK_METHOD(void,                    setStatus,         (int userId, const std::string& status), (override));
    MOCK_METHOD(void,                    assignManager,     (int userId, int managerUserId), (override));
    MOCK_METHOD(void,                    setFrozen,         (int userId, bool frozen), (override));
    MOCK_METHOD(bool,                    isFrozen,          (int userId), (override));
    MOCK_METHOD(bool,                    hasActiveAllocations, (int userId), (override));
    MOCK_METHOD(bool,                    existsByUserId,    (int userId), (override));
};
