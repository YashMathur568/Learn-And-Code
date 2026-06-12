#include "ManagerProjectsScreen.hpp"
#include "../core/AppSession.hpp"
#include "../core/ConsoleUtil.hpp"

#include <iostream>

static void showProjectDetail(const ApiClient& api, const nlohmann::json& proj) {
    while (true) {
        ConsoleUtil::clearScreen();
        const std::string health = proj.value("healthStatus", "ON_TRACK");

        std::cout << "── " << proj.value("name", "") << " ──\n";
        std::cout << "Health : " << ConsoleUtil::healthIcon(health) << " " << health << "\n";
        std::cout << "Status : " << proj.value("status", "") << "\n";
        std::cout << "Period : " << ConsoleUtil::fmtDate(proj.value("startDate", ""))
                  << "  →  "    << ConsoleUtil::fmtDate(proj.value("endDate", "")) << "\n";

        // Milestones
        const auto& ms = proj.value("milestones", nlohmann::json::array());
        if (!ms.empty()) {
            std::cout << "\nMilestones:\n";
            std::cout << ConsoleUtil::col("#",     4)
                      << ConsoleUtil::col("Title", 24)
                      << ConsoleUtil::col("Due",   12)
                      << "Status\n";
            ConsoleUtil::printSeparator();
            int milestoneIndex = 1;
            for (const auto& milestone : ms) {
                const std::string due    = milestone.value("dueDate", "");
                const std::string status = milestone.value("status", "");
                std::string title  = ConsoleUtil::trunc(milestone.value("title", ""), 22);
                const bool overdue = (status != "DONE" && due < ConsoleUtil::lastMonday());
                if (overdue) title += " ⚠";

                std::cout << ConsoleUtil::col(std::to_string(milestoneIndex++), 4)
                          << ConsoleUtil::col(title, 24)
                          << ConsoleUtil::col(ConsoleUtil::fmtDate(due), 12)
                          << status << (overdue ? "  OVERDUE" : "") << "\n";
            }
        }

        // Allocated resources
        const auto& allocs = proj.value("allocations", nlohmann::json::array());
        if (!allocs.empty()) {
            std::cout << "\nAllocated Resources:\n";
            std::cout << ConsoleUtil::col("Name",   22)
                      << ConsoleUtil::col("%",       5)
                      << ConsoleUtil::col("From",   12)
                      << "To\n";
            ConsoleUtil::printSeparator();
            for (const auto& allocation : allocs) {
                if (!allocation.value("isActive", false)) continue;
                std::cout << ConsoleUtil::col(allocation.value("employeeName", ""), 22)
                          << ConsoleUtil::col(std::to_string(allocation.value("allocationPercentage", 0)) + "%", 5)
                          << ConsoleUtil::col(ConsoleUtil::fmtDate(allocation.value("fromDate", "")), 12)
                          << ConsoleUtil::fmtDate(allocation.value("toDate", "")) << "\n";
            }
        }

        std::cout << "\n[A] AI Risk Summary   [B] Back\nOption: ";
        std::string opt;
        std::getline(std::cin, opt);

        if (opt == "b" || opt == "B") return;

        if (opt == "a" || opt == "A") {
            const int pid = proj.value("projectId", 0);
            std::cout << "\nGenerating AI summary...\n";
            const auto resp = api.post(
                "/api/manager/ai/risk-summary/" + std::to_string(pid),
                {},
                AppSession::get().token
            );
            ConsoleUtil::clearScreen();
            std::cout << "── AI Risk Summary — " << proj.value("name","") << " ──\n\n";
            if (!resp.success) {
                ConsoleUtil::printError(resp.errorMessage);
            } else {
                std::cout << "\"" << resp.body.value("summary", "") << "\"\n\n";
                ConsoleUtil::printInfo("Note: AI-generated from milestone and timesheet data.");
            }
            ConsoleUtil::pause();
        }
    }
}

void showManagerProjects(const ApiClient& api) {
    while (true) {
        ConsoleUtil::clearScreen();
        ConsoleUtil::printHeader("MY PROJECTS");

        const auto resp = api.get("/api/manager/projects", AppSession::get().token);
        if (!resp.success) { ConsoleUtil::printError(resp.errorMessage); ConsoleUtil::pause(); return; }

        const auto& projects = resp.body.value("data", nlohmann::json::array());

        if (projects.empty()) {
            ConsoleUtil::printInfo("No projects assigned to you.");
            ConsoleUtil::pause();
            return;
        }

        std::cout << ConsoleUtil::col("#",       4)
                  << ConsoleUtil::col("Project", 28)
                  << ConsoleUtil::col("End Date",12)
                  << "Health\n";
        ConsoleUtil::printSeparator();

        int projectIndex = 1;
        for (const auto& project : projects) {
            const std::string health = project.value("healthStatus", "ON_TRACK");
            std::cout << ConsoleUtil::col(std::to_string(projectIndex++), 4)
                      << ConsoleUtil::col(ConsoleUtil::trunc(project.value("name",""),27),28)
                      << ConsoleUtil::col(ConsoleUtil::fmtDate(project.value("endDate","")),12)
                      << ConsoleUtil::healthIcon(health) << " " << health << "\n";
        }
        ConsoleUtil::printSeparator();

        std::cout << "\nSelect project # for details, or [B] Back\nOption: ";
        std::string opt;
        std::getline(std::cin, opt);

        if (opt == "b" || opt == "B") return;

        int sel = 0; try { sel = std::stoi(opt) - 1; } catch (...) { continue; }
        if (sel < 0 || sel >= static_cast<int>(projects.size())) continue;

        const int pid = projects[sel].value("projectId", 0);
        const auto detailResp = api.get(
            "/api/manager/projects/" + std::to_string(pid),
            AppSession::get().token
        );
        if (!detailResp.success) { ConsoleUtil::printError(detailResp.errorMessage); ConsoleUtil::pause(); continue; }

        const auto& projDetail = detailResp.body.contains("data") ? detailResp.body["data"] : detailResp.body;
        showProjectDetail(api, projDetail);
    }
}
