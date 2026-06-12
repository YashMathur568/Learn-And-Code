#include "ManagerDashboardScreen.hpp"
#include "../core/AppSession.hpp"
#include "../core/ConsoleUtil.hpp"

#include <iostream>

void showManagerDashboard(const ApiClient& api) {
    while (true) {
        ConsoleUtil::clearScreen();
        ConsoleUtil::printHeader("RESOURCE DASHBOARD");

        const auto resp = api.get("/api/manager/dashboard", AppSession::get().token);
        if (!resp.success) { ConsoleUtil::printError(resp.errorMessage); ConsoleUtil::pause(); return; }

        const auto& body       = resp.body.contains("data") ? resp.body["data"] : resp.body;
        const auto& benchList  = body.value("benchEmployees",     nlohmann::json::array());
        const auto& activeList = body.value("allocatedEmployees", nlohmann::json::array());

        // ON BENCH
        std::cout << "ON BENCH  (" << benchList.size() << " employee(s) available)\n";
        ConsoleUtil::printSeparator();
        if (benchList.empty()) {
            ConsoleUtil::printInfo("No employees on bench.");
        } else {
            std::cout << ConsoleUtil::col("ID",   6)
                      << ConsoleUtil::col("Name", 22)
                      << ConsoleUtil::col("Dept", 14)
                      << "Top Skills\n";
            ConsoleUtil::printSeparator();
            for (const auto& emp : benchList) {
                std::string skills;
                for (const auto& skill : emp.value("skills", nlohmann::json::array())) {
                    if (!skills.empty()) skills += ", ";
                    skills += skill.value("skillName", "");
                }
                std::cout << ConsoleUtil::col(std::to_string(emp.value("userId", 0)), 6)
                          << ConsoleUtil::col(emp.value("fullName", ""), 22)
                          << ConsoleUtil::col(emp.value("department", ""), 14)
                          << ConsoleUtil::trunc(skills, 28) << "\n";
            }
        }

        // ACTIVE / PARTIALLY ALLOCATED
        std::cout << "\nACTIVE EMPLOYEES\n";
        ConsoleUtil::printSeparator();
        if (activeList.empty()) {
            ConsoleUtil::printInfo("No allocated employees.");
        } else {
            std::cout << ConsoleUtil::col("ID",   6)
                      << ConsoleUtil::col("Name", 22)
                      << ConsoleUtil::col("Alloc%", 8)
                      << "Availability\n";
            ConsoleUtil::printSeparator();
            for (const auto& emp : activeList) {
                const int pct  = emp.value("totalAllocationPct", 0);
                const int free = 100 - pct;
                const std::string avail = free <= 0 ? "FULL" : std::to_string(free) + "% free";
                std::cout << ConsoleUtil::col(std::to_string(emp.value("userId", 0)), 6)
                          << ConsoleUtil::col(emp.value("fullName", ""), 22)
                          << ConsoleUtil::col(std::to_string(pct) + "%", 8)
                          << avail << "\n";
            }
        }

        ConsoleUtil::printSeparator();
        std::cout << "Bench: " << benchList.size()
                  << "   |   Active: " << activeList.size() << "\n";

        std::cout << "\n[D] Drill into employee   [B] Back\nOption: ";
        std::string opt;
        std::getline(std::cin, opt);

        if (opt == "b" || opt == "B") return;

        if (opt == "d" || opt == "D") {
            const std::string idStr = ConsoleUtil::promptInput("Employee ID: ");
            if (idStr.empty()) continue;

            const auto empResp = api.get("/api/manager/employees/" + idStr, AppSession::get().token);
            if (!empResp.success) { ConsoleUtil::printError(empResp.errorMessage); ConsoleUtil::pause(); continue; }

            ConsoleUtil::clearScreen();
            const auto& employeeData = empResp.body.contains("data") ? empResp.body["data"] : empResp.body;

            std::cout << "\n── " << employeeData.value("fullName", "") << " ──\n\n";
            std::cout << "Department     : " << employeeData.value("department", "") << "\n";
            std::cout << "Designation    : " << employeeData.value("designation", "") << "\n";
            std::cout << "Status         : " << employeeData.value("status", "") << "\n";

            std::string skillStr;
            for (const auto& skill : employeeData.value("skills", nlohmann::json::array())) {
                if (!skillStr.empty()) skillStr += ", ";
                skillStr += skill.value("skillName", "");
            }
            std::cout << "Profile Skills : " << skillStr << "\n";

            const auto& allocs = employeeData.value("activeAllocations", nlohmann::json::array());
            if (!allocs.empty()) {
                std::cout << "\nActive Allocations:\n";
                std::cout << ConsoleUtil::col("  Project",   24)
                          << ConsoleUtil::col("%", 5)
                          << ConsoleUtil::col("From", 12)
                          << "To\n";
                ConsoleUtil::printSeparator();
                for (const auto& allocation : allocs) {
                    std::cout << "  "
                              << ConsoleUtil::col(ConsoleUtil::trunc(allocation.value("projectName",""),22),24)
                              << ConsoleUtil::col(std::to_string(allocation.value("allocationPercentage",0))+"%",5)
                              << ConsoleUtil::col(ConsoleUtil::fmtDate(allocation.value("fromDate","")),12)
                              << ConsoleUtil::fmtDate(allocation.value("toDate","")) << "\n";
                }
            }

            ConsoleUtil::pause();
        }
    }
}
