#pragma once

#include "../services/IAllocationService.hpp"
#include "../services/ITimesheetService.hpp"
#include "../repositories/IProjectRepository.hpp"
#include <drogon/HttpController.h>
#include <memory>

class EmployeeApiController : public drogon::HttpController<EmployeeApiController> {
public:
    EmployeeApiController();

    METHOD_LIST_BEGIN
        ADD_METHOD_TO(EmployeeApiController::getMyAllocations,  "/api/employee/allocations", drogon::Get,  "JwtMiddleware");
        ADD_METHOD_TO(EmployeeApiController::getMyTimesheets,   "/api/employee/timesheets",  drogon::Get,  "JwtMiddleware");
        ADD_METHOD_TO(EmployeeApiController::submitTimesheet,   "/api/employee/timesheets",  drogon::Post, "JwtMiddleware");
    METHOD_LIST_END

    void getMyAllocations(
        const drogon::HttpRequestPtr& request,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback
    );

    void getMyTimesheets(
        const drogon::HttpRequestPtr& request,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback
    );

    void submitTimesheet(
        const drogon::HttpRequestPtr& request,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback
    );

private:
    std::shared_ptr<IAllocationService>  allocationService;
    std::shared_ptr<ITimesheetService>   timesheetService;
    std::shared_ptr<IProjectRepository>  projectRepository;
};
