#include "ManagerDashboardScreen.hpp"
#include "AppSession.hpp"
#include "ConsoleUtil.hpp"

#include <iostream>

void showManagerDashboard(const ApiClient& api) {
    while (true) {
        ConsoleUtil::clearScreen();
        ConsoleUtil::printHeader("RESOURCE DASHBOARD");

        const auto resp = api.get("/api/manager/dashboard", AppSession::get().token);
        if (!resp.success) { ConsoleUtil::printError(resp.errorMessage); ConsoleUtil::pause(); return; }

        const auto& body       = resp.body.contains("data") ? resp.body["data"] : resp.body;
        const auto& benchList  = body.value("benchResources",     nlohmann::json::array());
        const auto& activeList = body.value("allocatedResources", nlohmann::json::array());


        std::cout << "ON BENCH  (" << benchList.size() << " resource(s) available)\n";
        ConsoleUtil::printSeparator();
        if (benchList.empty()) {
            ConsoleUtil::printInfo("No resources on bench.");
        } else {
            std::cout << ConsoleUtil::col("ID",   6)
                      << ConsoleUtil::col("Name", 22)
                      << ConsoleUtil::col("Dept", 14)
                      << "Top Skills\n";
            ConsoleUtil::printSeparator();
            for (const auto& employee : benchList) {
                std::string skills;
                for (const auto& skill : employee.value("skills", nlohmann::json::array())) {
                    if (!skills.empty()) skills += ", ";
                    skills += skill.value("skillName", "");
                }
                std::cout << ConsoleUtil::col(std::to_string(employee.value("userId", 0)), 6)
                          << ConsoleUtil::col(employee.value("fullName", ""), 22)
                          << ConsoleUtil::col(employee.value("department", ""), 14)
                          << ConsoleUtil::trunc(skills, 28) << "\n";
            }
        }


        std::cout << "\nACTIVE RESOURCES\n";
        ConsoleUtil::printSeparator();
        if (activeList.empty()) {
            ConsoleUtil::printInfo("No allocated resources.");
        } else {
            std::cout << ConsoleUtil::col("ID",   6)
                      << ConsoleUtil::col("Name", 22)
                      << ConsoleUtil::col("Alloc%", 8)
                      << "Availability\n";
            ConsoleUtil::printSeparator();
            for (const auto& employee : activeList) {
                const int pct  = employee.value("totalAllocationPct", 0);
                const int free = 100 - pct;
                const std::string avail = free <= 0 ? "FULL" : std::to_string(free) + "% free";
                std::cout << ConsoleUtil::col(std::to_string(employee.value("userId", 0)), 6)
                          << ConsoleUtil::col(employee.value("fullName", ""), 22)
                          << ConsoleUtil::col(std::to_string(pct) + "%", 8)
                          << avail << "\n";
            }
        }

        ConsoleUtil::printSeparator();
        std::cout << "Bench: " << benchList.size()
                  << "   |   Active: " << activeList.size() << "\n";

        std::cout << "\n[D] Drill into resource   [B] Back\nOption: ";
        std::string selectedOption;
        std::getline(std::cin, selectedOption);

        if (selectedOption == "b" || selectedOption == "B") return;

        if (selectedOption == "d" || selectedOption == "D") {
            const std::string idStr = ConsoleUtil::promptInput("Resource ID: ");
            if (idStr.empty()) continue;

            const auto empResp = api.get("/api/manager/resources/" + idStr, AppSession::get().token);
            if (!empResp.success) { ConsoleUtil::printError(empResp.errorMessage); ConsoleUtil::pause(); continue; }

            ConsoleUtil::clearScreen();
            const auto& resourceData = empResp.body.contains("data") ? empResp.body["data"] : empResp.body;

            std::cout << "\n── " << resourceData.value("fullName", "") << " ──\n\n";
            std::cout << "Department     : " << resourceData.value("department", "") << "\n";
            std::cout << "Designation    : " << resourceData.value("designation", "") << "\n";
            std::cout << "Status         : " << resourceData.value("status", "") << "\n";

            std::string skillStr;
            for (const auto& skill : resourceData.value("skills", nlohmann::json::array())) {
                if (!skillStr.empty()) skillStr += ", ";
                skillStr += skill.value("skillName", "");
            }
            std::cout << "Profile Skills : " << skillStr << "\n";

            const auto& allocs = resourceData.value("activeAllocations", nlohmann::json::array());
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
          else {
              ConsoleUtil::printError("Invalid option. Press D to drill into a resource or B to go back.");
              ConsoleUtil::pause();
          }
      }
}
