#include "EmployeeAllocationsScreen.hpp"
#include "../core/AppSession.hpp"
#include "../core/ConsoleUtil.hpp"

#include <iostream>

void showMyAllocations(const ApiClient& api) {
    ConsoleUtil::clearScreen();
    ConsoleUtil::printHeader("MY ALLOCATIONS");

    const auto resp = api.get("/api/employee/allocations", AppSession::get().token);
    if (!resp.success) { ConsoleUtil::printError(resp.errorMessage); ConsoleUtil::pause(); return; }

    const auto& data = resp.body.value("data", nlohmann::json::array());

    std::cout << ConsoleUtil::col("Project",  28)
              << ConsoleUtil::col("%",          5)
              << ConsoleUtil::col("From",       12)
              << ConsoleUtil::col("To",         12)
              << "Status\n";
    ConsoleUtil::printSeparator();

    int totalPct = 0;
    for (const auto& allocation : data) {
        if (!allocation.value("isActive", false)) continue;
        const int pct = allocation.value("allocationPercentage", 0);
        totalPct += pct;
        std::cout << ConsoleUtil::col(ConsoleUtil::trunc(allocation.value("projectName",""),27), 28)
                  << ConsoleUtil::col(std::to_string(pct) + "%", 5)
                  << ConsoleUtil::col(ConsoleUtil::fmtDate(allocation.value("fromDate","")), 12)
                  << ConsoleUtil::col(ConsoleUtil::fmtDate(allocation.value("toDate","")),   12)
                  << "ACTIVE\n";
    }
    ConsoleUtil::printSeparator();
    std::cout << "Total Utilisation: " << totalPct << "%\n";

    ConsoleUtil::pause();
}
