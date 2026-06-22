#pragma once

#include "IAllocationService.hpp"
#include "ITimesheetService.hpp"
#include "IProjectRepository.hpp"
#include "IResourceRepository.hpp"
#include <drogon/HttpController.h>
#include <memory>

class ResourceApiController : public drogon::HttpController<ResourceApiController> {
public:
    ResourceApiController();

    METHOD_LIST_BEGIN
        ADD_METHOD_TO(ResourceApiController::getCurrentResource, "/api/resource/current",     drogon::Get,  "JwtMiddleware");
        ADD_METHOD_TO(ResourceApiController::getMyAllocations,  "/api/resource/allocations", drogon::Get,  "JwtMiddleware");
        ADD_METHOD_TO(ResourceApiController::getMyTimesheets,   "/api/resource/timesheets",  drogon::Get,  "JwtMiddleware");
        ADD_METHOD_TO(ResourceApiController::submitTimesheet,   "/api/resource/timesheets",  drogon::Post, "JwtMiddleware");
    METHOD_LIST_END

    void getCurrentResource(
        const drogon::HttpRequestPtr& request,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback
    );

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
    std::shared_ptr<IResourceRepository> resourceRepository;
};
