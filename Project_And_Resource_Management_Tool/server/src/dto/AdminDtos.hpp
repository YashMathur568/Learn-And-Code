#pragma once

#include "../models/User.hpp"
#include "../models/Employee.hpp"
#include "../models/EmployeeSkill.hpp"
#include <nlohmann/json.hpp>
#include <string>

struct CreateUserRequest {
    std::string fullName;
    std::string email;
    std::string username;
    std::string tempPassword;
    std::string role;
    std::string department;
    std::string designation;

    static CreateUserRequest fromJson(const nlohmann::json& body) {
        CreateUserRequest request;
        request.fullName     = body.at("fullName").get<std::string>();
        request.email        = body.at("email").get<std::string>();
        request.username     = body.at("username").get<std::string>();
        request.tempPassword = body.at("tempPassword").get<std::string>();
        request.role         = body.at("role").get<std::string>();
        request.department   = body.value("department", "");
        request.designation  = body.value("designation", "");
        return request;
    }
};

struct ResetPasswordRequest {
    std::string tempPassword;

    static ResetPasswordRequest fromJson(const nlohmann::json& body) {
        ResetPasswordRequest request;
        request.tempPassword = body.at("tempPassword").get<std::string>();
        return request;
    }
};

struct AssignManagerRequest {
    int managerEmployeeId{0};

    static AssignManagerRequest fromJson(const nlohmann::json& body) {
        AssignManagerRequest request;
        request.managerEmployeeId = body.at("managerEmployeeId").get<int>();
        return request;
    }
};

struct UpdateEmployeeRequest {
    std::string fullName;
    std::string email;
    std::string department;
    std::string designation;

    static UpdateEmployeeRequest fromJson(const nlohmann::json& body) {
        UpdateEmployeeRequest request;
        request.fullName    = body.at("fullName").get<std::string>();
        request.email       = body.at("email").get<std::string>();
        request.department  = body.at("department").get<std::string>();
        request.designation = body.at("designation").get<std::string>();
        return request;
    }
};

struct SkillRequest {
    std::string skillName;
    std::string category;
    std::string proficiency;

    static SkillRequest fromJson(const nlohmann::json& body) {
        SkillRequest request;
        request.skillName   = body.at("skillName").get<std::string>();
        request.category    = body.at("category").get<std::string>();
        request.proficiency = body.at("proficiency").get<std::string>();
        return request;
    }
};

inline nlohmann::json userToJson(const User& user) {
    return {
        {"userId",             user.userId},
        {"fullName",           user.fullName},
        {"email",              user.email},
        {"username",           user.username},
        {"role",               user.role},
        {"isActive",           user.isActive},
        {"forcePasswordChange",user.forcePwdChange},
        {"createdAt",          user.createdAt}
    };
}

inline nlohmann::json employeeToJson(const Employee& employee) {
    nlohmann::json json = {
        {"employeeId",  employee.employeeId},
        {"userId",      employee.userId},
        {"fullName",    employee.fullName},
        {"email",       employee.email},
        {"department",  employee.department},
        {"designation", employee.designation},
        {"status",      employee.status},
        {"isActive",    employee.isActive}
    };
    if (employee.managerId > 0) {
        json["managerId"] = employee.managerId;
    } else {
        json["managerId"] = nullptr;
    }
    return json;
}

inline nlohmann::json skillToJson(const EmployeeSkill& skill) {
    return {
        {"skillId",     skill.skillId},
        {"employeeId",  skill.employeeId},
        {"skillName",   skill.skillName},
        {"category",    skill.category},
        {"proficiency", skill.proficiency}
    };
}
