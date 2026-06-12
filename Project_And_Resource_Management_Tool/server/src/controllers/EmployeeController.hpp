#pragma once

#include "../services/IEmployeeService.hpp"
#include "../repositories/IAllocationRepository.hpp"
#include <drogon/HttpController.h>
#include <memory>

class EmployeeController : public drogon::HttpController<EmployeeController> {
public:
    EmployeeController();

    METHOD_LIST_BEGIN
        ADD_METHOD_TO(EmployeeController::getAllEmployees,    "/api/admin/employees",                          drogon::Get,    "JwtMiddleware");
        ADD_METHOD_TO(EmployeeController::updateEmployee,    "/api/admin/employees/{id}",                     drogon::Put,    "JwtMiddleware");
        ADD_METHOD_TO(EmployeeController::deactivateEmployee,"/api/admin/employees/{id}/deactivate",          drogon::Put,    "JwtMiddleware");
        ADD_METHOD_TO(EmployeeController::assignManager,     "/api/admin/employees/{id}/assign-manager",      drogon::Put,    "JwtMiddleware");
        ADD_METHOD_TO(EmployeeController::getSkills,         "/api/admin/employees/{id}/skills",              drogon::Get,    "JwtMiddleware");
        ADD_METHOD_TO(EmployeeController::addSkill,          "/api/admin/employees/{id}/skills",              drogon::Post,   "JwtMiddleware");
        ADD_METHOD_TO(EmployeeController::updateSkill,       "/api/admin/employees/{id}/skills/{skillId}",    drogon::Put,    "JwtMiddleware");
        ADD_METHOD_TO(EmployeeController::removeSkill,       "/api/admin/employees/{id}/skills/{skillId}",    drogon::Delete, "JwtMiddleware");
        ADD_METHOD_TO(EmployeeController::restoreAccess,     "/api/admin/employees/{id}/restore-access",       drogon::Put,    "JwtMiddleware");
    METHOD_LIST_END

    void getAllEmployees(
        const drogon::HttpRequestPtr& request,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback
    );

    void updateEmployee(
        const drogon::HttpRequestPtr& request,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback,
        int id
    );

    void deactivateEmployee(
        const drogon::HttpRequestPtr& request,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback,
        int id
    );

    void assignManager(
        const drogon::HttpRequestPtr& request,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback,
        int id
    );

    void getSkills(
        const drogon::HttpRequestPtr& request,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback,
        int id
    );

    void addSkill(
        const drogon::HttpRequestPtr& request,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback,
        int id
    );

    void updateSkill(
        const drogon::HttpRequestPtr& request,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback,
        int id,
        int skillId
    );

    void removeSkill(
        const drogon::HttpRequestPtr& request,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback,
        int id,
        int skillId
    );

    void restoreAccess(
        const drogon::HttpRequestPtr& request,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback,
        int id
    );

private:
    std::shared_ptr<IEmployeeService>    employeeService;
    std::shared_ptr<IAllocationRepository> allocationRepository;
};
