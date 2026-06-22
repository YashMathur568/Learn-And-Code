#pragma once

#include <string>
#include <vector>

namespace Department {
    constexpr const char* ENGINEERING          = "Engineering";
    constexpr const char* PRODUCT              = "Product";
    constexpr const char* DESIGN               = "Design";
    constexpr const char* QA_TESTING           = "QA / Testing";
    constexpr const char* DEVOPS_INFRA         = "DevOps / Infrastructure";
    constexpr const char* DATA_ANALYTICS       = "Data & Analytics";
    constexpr const char* SECURITY             = "Security";
    constexpr const char* ARCHITECTURE         = "Architecture";
    constexpr const char* PROJECT_MANAGEMENT   = "Project Management";
    constexpr const char* MANAGEMENT           = "Management";
    constexpr const char* ADMINISTRATION       = "Administration";
    constexpr const char* HR_OPERATIONS        = "HR & Operations";
    constexpr const char* FINANCE              = "Finance";
    constexpr const char* SALES_BD             = "Sales & Business Development";

    inline const std::vector<std::string>& values() {
        static const std::vector<std::string> departments = {
            ENGINEERING, PRODUCT, DESIGN, QA_TESTING, DEVOPS_INFRA,
            DATA_ANALYTICS, SECURITY, ARCHITECTURE, PROJECT_MANAGEMENT,
            MANAGEMENT, ADMINISTRATION, HR_OPERATIONS, FINANCE, SALES_BD
        };
        return departments;
    }

    inline bool isValid(const std::string& departmentName) {
        for (const auto& dept : values()) if (dept == departmentName) return true;
        return false;
    }
}
