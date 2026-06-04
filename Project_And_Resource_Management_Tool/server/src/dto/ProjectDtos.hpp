#pragma once

#include "../models/Project.hpp"
#include "../models/Milestone.hpp"
#include "../models/SystemConfig.hpp"
#include <nlohmann/json.hpp>
#include <string>

struct CreateProjectRequest {
    std::string name;
    std::string description;
    std::string startDate;
    std::string endDate;
    std::string status;
    int         managerId{0};

    static CreateProjectRequest fromJson(const nlohmann::json& body) {
        CreateProjectRequest request;
        request.name        = body.at("name").get<std::string>();
        request.description = body.value("description", "");
        request.startDate   = body.at("startDate").get<std::string>();
        request.endDate     = body.at("endDate").get<std::string>();
        request.status      = body.value("status", "PLANNED");
        request.managerId   = body.at("managerId").get<int>();
        return request;
    }
};

struct UpdateProjectRequest {
    std::string name;
    std::string description;
    std::string startDate;
    std::string endDate;
    std::string status;
    int         managerId{0};

    static UpdateProjectRequest fromJson(const nlohmann::json& body) {
        UpdateProjectRequest request;
        request.name        = body.at("name").get<std::string>();
        request.description = body.value("description", "");
        request.startDate   = body.at("startDate").get<std::string>();
        request.endDate     = body.at("endDate").get<std::string>();
        request.status      = body.at("status").get<std::string>();
        request.managerId   = body.at("managerId").get<int>();
        return request;
    }
};

struct CreateMilestoneRequest {
    std::string title;
    std::string dueDate;
    std::string status;

    static CreateMilestoneRequest fromJson(const nlohmann::json& body) {
        CreateMilestoneRequest request;
        request.title   = body.at("title").get<std::string>();
        request.dueDate = body.at("dueDate").get<std::string>();
        request.status  = body.value("status", "NOT_STARTED");
        return request;
    }
};

struct UpdateMilestoneRequest {
    std::string title;
    std::string dueDate;
    std::string status;

    static UpdateMilestoneRequest fromJson(const nlohmann::json& body) {
        UpdateMilestoneRequest request;
        request.title   = body.at("title").get<std::string>();
        request.dueDate = body.at("dueDate").get<std::string>();
        request.status  = body.at("status").get<std::string>();
        return request;
    }
};

struct UpdateConfigRequest {
    std::string llmProvider;
    std::string schedulerIntervalHours;
    std::string maxWeeklyHours;

    static UpdateConfigRequest fromJson(const nlohmann::json& body) {
        UpdateConfigRequest request;
        request.llmProvider             = body.value("llmProvider", "");
        request.schedulerIntervalHours  = body.value("schedulerIntervalHours", "");
        request.maxWeeklyHours          = body.value("maxWeeklyHours", "");
        return request;
    }
};

inline nlohmann::json projectToJson(const Project& project) {
    return {
        {"projectId",   project.projectId},
        {"name",        project.name},
        {"description", project.description},
        {"startDate",   project.startDate},
        {"endDate",     project.endDate},
        {"status",      project.status},
        {"managerId",   project.managerId},
        {"health",      project.health}
    };
}

inline nlohmann::json milestoneToJson(const Milestone& milestone) {
    return {
        {"milestoneId", milestone.milestoneId},
        {"projectId",   milestone.projectId},
        {"title",       milestone.title},
        {"dueDate",     milestone.dueDate},
        {"status",      milestone.status}
    };
}
