#pragma once

#include "../services/IAllocationService.hpp"
#include "../services/IProjectService.hpp"
#include "../services/IEmployeeService.hpp"
#include "../services/ITimesheetService.hpp"
#include <drogon/HttpController.h>
#include <memory>

class ManagerController : public drogon::HttpController<ManagerController> {
public:
    ManagerController();

    METHOD_LIST_BEGIN
        ADD_METHOD_TO(ManagerController::getDashboard,       "/api/manager/dashboard",                    drogon::Get,  "JwtMiddleware");
        ADD_METHOD_TO(ManagerController::getEmployeeDetail,  "/api/manager/employees/{id}",               drogon::Get,  "JwtMiddleware");
        ADD_METHOD_TO(ManagerController::createAllocation,   "/api/manager/allocations",                  drogon::Post, "JwtMiddleware");
        ADD_METHOD_TO(ManagerController::endAllocation,      "/api/manager/allocations/{id}/end",         drogon::Put,  "JwtMiddleware");
        ADD_METHOD_TO(ManagerController::getProjects,        "/api/manager/projects",                     drogon::Get,  "JwtMiddleware");
        ADD_METHOD_TO(ManagerController::getProjectDetail,   "/api/manager/projects/{id}",                drogon::Get,  "JwtMiddleware");
        ADD_METHOD_TO(ManagerController::getTeamTimesheets,  "/api/manager/timesheets",                   drogon::Get,  "JwtMiddleware");
    METHOD_LIST_END

    void getDashboard(
        const drogon::HttpRequestPtr& request,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback
    );

    void getEmployeeDetail(
        const drogon::HttpRequestPtr& request,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback,
        int id
    );

    void createAllocation(
        const drogon::HttpRequestPtr& request,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback
    );

    void endAllocation(
        const drogon::HttpRequestPtr& request,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback,
        int id
    );

    void getProjects(
        const drogon::HttpRequestPtr& request,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback
    );

    void getProjectDetail(
        const drogon::HttpRequestPtr& request,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback,
        int id
    );

    void getTeamTimesheets(
        const drogon::HttpRequestPtr& request,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback
    );

private:
    std::shared_ptr<IAllocationService> allocationService;
    std::shared_ptr<IProjectService>    projectService;
    std::shared_ptr<IEmployeeService>   employeeService;
    std::shared_ptr<ITimesheetService>  timesheetService;
};
