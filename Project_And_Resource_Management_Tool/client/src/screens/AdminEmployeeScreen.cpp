#include "AdminEmployeeScreen.hpp"
#include "../core/AppSession.hpp"
#include "../core/ConsoleUtil.hpp"

#include <iostream>
#include <algorithm>

// ── View all employees ────────────────────────────────────────────────────────
static void viewAllEmployees(const ApiClient& api) {
    while (true) {
        ConsoleUtil::clearScreen();
        ConsoleUtil::printHeader("ALL EMPLOYEES");

        const auto resp = api.get("/api/admin/employees", AppSession::get().token);
        if (!resp.success) {
            ConsoleUtil::printError(resp.errorMessage);
            ConsoleUtil::pause();
            return;
        }

        const auto& data = resp.body.value("data", nlohmann::json::array());

        std::cout << ConsoleUtil::col("ID",   6)
              << ConsoleUtil::col("Name", 20)
                  << ConsoleUtil::col("Department", 14)
              << ConsoleUtil::col("Designation", 28)
                  << "Status\n";
        ConsoleUtil::printSeparator();

        int allocated = 0, bench = 0;
        for (const auto& emp : data) {
            const std::string status = emp.value("status", "");
            if (status == "ALLOCATED") ++allocated; else ++bench;

            std::cout << ConsoleUtil::col(std::to_string(emp.value("employeeId", 0)), 6)
                      << ConsoleUtil::col(emp.value("fullName", ""), 20)
                      << ConsoleUtil::col(emp.value("department", ""), 14)
                      << ConsoleUtil::col(emp.value("designation", ""), 28)
                      << status << "\n";
        }
        ConsoleUtil::printSeparator();
        std::cout << "Total: " << data.size()
                  << "   |   Allocated: " << allocated
                  << "   |   Bench: " << bench << "\n";

        std::cout << "\n[B] Back\n\nOption: ";
        std::string opt;
        std::getline(std::cin, opt);
        if (opt == "b" || opt == "B") return;
    }
}

// ── Update employee ───────────────────────────────────────────────────────────
static void updateEmployee(const ApiClient& api) {
    ConsoleUtil::clearScreen();
    ConsoleUtil::printHeader("UPDATE EMPLOYEE");

    const std::string idStr = ConsoleUtil::promptInput("Employee ID: ");
    if (idStr.empty() || idStr == "b" || idStr == "B") return;

    const auto respGet = api.get("/api/admin/employees", AppSession::get().token);
    if (!respGet.success) { ConsoleUtil::printError(respGet.errorMessage); ConsoleUtil::pause(); return; }

    nlohmann::json found;
    for (const auto& e : respGet.body.value("data", nlohmann::json::array())) {
        if (std::to_string(e.value("employeeId", 0)) == idStr) { found = e; break; }
    }
    if (found.is_null()) { ConsoleUtil::printError("Employee not found."); ConsoleUtil::pause(); return; }

    const std::string name  = found.value("fullName", "");
    const std::string email = found.value("email", "");
    std::cout << "\n── " << name << " ──\n";
    std::cout << "Leave any field blank to keep current value.\n\n";

    const std::string fullName = ConsoleUtil::promptInput("Full Name      [" + name + "]: ");
    const std::string emailIn  = ConsoleUtil::promptInput("Email          [" + email + "]: ");
    const std::string dept    = ConsoleUtil::promptInput("Department     [" + found.value("department", "") + "]: ");
    const std::string desig   = ConsoleUtil::promptInput("Designation    [" + found.value("designation", "") + "]: ");

    const std::string newFullName    = fullName.empty() ? name : fullName;
    const std::string newEmail       = emailIn.empty() ? email : emailIn;
    const std::string newDepartment  = dept.empty() ? found.value("department", "") : dept;
    const std::string newDesignation = desig.empty() ? found.value("designation", "") : desig;

    if (newFullName == name
        && newEmail == email
        && newDepartment == found.value("department", "")
        && newDesignation == found.value("designation", "")) {
        ConsoleUtil::printInfo("No changes made.");
        ConsoleUtil::pause();
        return;
    }

    nlohmann::json payload = {
        {"fullName", newFullName},
        {"email", newEmail},
        {"department", newDepartment},
        {"designation", newDesignation}
    };

    const auto resp = api.put("/api/admin/employees/" + idStr, payload, AppSession::get().token);
    if (!resp.success) { ConsoleUtil::printError(resp.errorMessage); }
    else               { ConsoleUtil::printSuccess("Employee updated."); }
    ConsoleUtil::pause();
}

// ── Deactivate employee ───────────────────────────────────────────────────────
static void deactivateEmployee(const ApiClient& api) {
    ConsoleUtil::clearScreen();
    ConsoleUtil::printHeader("DEACTIVATE EMPLOYEE");

    const std::string idStr = ConsoleUtil::promptInput("Employee ID: ");
    if (idStr.empty() || idStr == "b" || idStr == "B") return;

    const auto respGet = api.get("/api/admin/employees", AppSession::get().token);
    if (!respGet.success) { ConsoleUtil::printError(respGet.errorMessage); ConsoleUtil::pause(); return; }

    nlohmann::json found;
    for (const auto& e : respGet.body.value("data", nlohmann::json::array())) {
        if (std::to_string(e.value("employeeId", 0)) == idStr) { found = e; break; }
    }
    if (found.is_null()) { ConsoleUtil::printError("Employee not found."); ConsoleUtil::pause(); return; }

    const std::string name   = found.value("fullName", "");
    const std::string status = found.value("status", "");
    const std::string dept   = found.value("department", "");

    std::cout << "\n── " << name << " ──\n";
    std::cout << "Department : " << dept << "\n";
    std::cout << "Status     : " << status << "\n\n";
    ConsoleUtil::printWarning("This will end all active allocations and block login.");

    if (!ConsoleUtil::confirm("Deactivate " + name + "?")) {
        ConsoleUtil::printInfo("Cancelled."); ConsoleUtil::pause(); return;
    }

    const auto resp = api.put("/api/admin/employees/" + idStr + "/deactivate", {}, AppSession::get().token);
    if (!resp.success) { ConsoleUtil::printError(resp.errorMessage); }
    else {
        ConsoleUtil::printSuccess("Employee deactivated.");
        const auto& ended = resp.body.value("endedAllocations", nlohmann::json::array());
        if (!ended.empty()) {
            std::cout << "  Ended allocations: " << ended.size() << "\n";
        }
    }
    ConsoleUtil::pause();
}

// ── Assign manager ────────────────────────────────────────────────────────────
static void assignManager(const ApiClient& api) {
    ConsoleUtil::clearScreen();
    ConsoleUtil::printHeader("ASSIGN MANAGER");

    const std::string empId = ConsoleUtil::promptInput("Employee ID  : ");
    if (empId.empty() || empId == "b" || empId == "B") return;
    const std::string mgrId = ConsoleUtil::promptInput("Manager ID   : ");
    if (mgrId.empty()) { ConsoleUtil::printError("Manager ID required."); ConsoleUtil::pause(); return; }

    const auto resp = api.put(
        "/api/admin/employees/" + empId + "/assign-manager",
        {{"managerEmployeeId", std::stoi(mgrId)}},
        AppSession::get().token
    );
    if (!resp.success) { ConsoleUtil::printError(resp.errorMessage); }
    else               { ConsoleUtil::printSuccess("Manager assigned."); }
    ConsoleUtil::pause();
}

// ── Manage skills ─────────────────────────────────────────────────────────────
static void manageSkills(const ApiClient& api) {
    ConsoleUtil::clearScreen();
    ConsoleUtil::printHeader("MANAGE EMPLOYEE SKILLS");

    const std::string idStr = ConsoleUtil::promptInput("Employee ID: ");
    if (idStr.empty() || idStr == "b" || idStr == "B") return;

    while (true) {
        ConsoleUtil::clearScreen();
        ConsoleUtil::printHeader("MANAGE SKILLS");
        std::cout << "Employee ID: " << idStr << "\n\n";

        const auto resp = api.get("/api/admin/employees/" + idStr + "/skills", AppSession::get().token);
        if (!resp.success) { ConsoleUtil::printError(resp.errorMessage); ConsoleUtil::pause(); return; }

        const auto& skills = resp.body.value("data", nlohmann::json::array());
        if (skills.empty()) {
            std::cout << "  (no skills on record)\n";
        } else {
            int i = 1;
            std::cout << ConsoleUtil::col("#", 4)
                      << ConsoleUtil::col("Skill", 24)
                      << ConsoleUtil::col("Category", 14)
                      << "Proficiency\n";
            ConsoleUtil::printSeparator();
            for (const auto& s : skills) {
                std::cout << ConsoleUtil::col(std::to_string(i++), 4)
                          << ConsoleUtil::col(s.value("skillName", ""), 24)
                          << ConsoleUtil::col(s.value("category", ""), 14)
                          << s.value("proficiency", "") << "\n";
            }
        }

        std::cout << "\n1. Add Skill\n2. Update Proficiency\n3. Remove Skill\n4. Back\n\nOption: ";
        std::string opt;
        std::getline(std::cin, opt);

        if (opt == "4" || opt == "b" || opt == "B") return;

        if (opt == "1") {
            ConsoleUtil::clearScreen();
            ConsoleUtil::printHeader("ADD SKILL");
            const std::string skillName = ConsoleUtil::promptInput("Skill Name         : ");
            if (skillName.empty()) continue;

            std::cout << "Category  : (1) Backend  (2) Frontend  (3) DevOps  (4) QA  (5) Other\n";
            const std::string catOpt = ConsoleUtil::promptInput("Choice             : ");
            const std::vector<std::string> cats = {"Backend","Frontend","DevOps","QA","Other"};
            const std::string category = (catOpt >= "1" && catOpt <= "5") ? cats[std::stoi(catOpt) - 1] : "Other";

            std::cout << "Proficiency: (1) Beginner  (2) Intermediate  (3) Advanced\n";
            const std::string profOpt = ConsoleUtil::promptInput("Choice             : ");
            const std::vector<std::string> profs = {"Beginner","Intermediate","Advanced"};
            const std::string proficiency = (profOpt >= "1" && profOpt <= "3") ? profs[std::stoi(profOpt) - 1] : "Beginner";

            const auto addResp = api.post(
                "/api/admin/employees/" + idStr + "/skills",
                {{"skillName", skillName}, {"category", category}, {"proficiency", proficiency}},
                AppSession::get().token
            );
            if (!addResp.success) ConsoleUtil::printError(addResp.errorMessage);
            else                  ConsoleUtil::printSuccess("Skill added.");
            ConsoleUtil::pause();

        } else if (opt == "2" && !skills.empty()) {
            const std::string numStr = ConsoleUtil::promptInput("Skill # to update  : ");
            int idx = 0;
            try { idx = std::stoi(numStr) - 1; } catch (...) { continue; }
            if (idx < 0 || idx >= static_cast<int>(skills.size())) continue;

            const int skillId = skills[idx].value("skillId", 0);
            const std::string skillName = skills[idx].value("skillName", "");
            const std::string category = skills[idx].value("category", "Other");
            std::cout << "Proficiency: (1) Beginner  (2) Intermediate  (3) Advanced\n";
            const std::string profOpt = ConsoleUtil::promptInput("Choice             : ");
            const std::vector<std::string> profs = {"Beginner","Intermediate","Advanced"};
            const std::string proficiency = (profOpt >= "1" && profOpt <= "3") ? profs[std::stoi(profOpt) - 1] : "Beginner";

            const auto updResp = api.put(
                "/api/admin/employees/" + idStr + "/skills/" + std::to_string(skillId),
                {{"skillName", skillName}, {"category", category}, {"proficiency", proficiency}},
                AppSession::get().token
            );
            if (!updResp.success) ConsoleUtil::printError(updResp.errorMessage);
            else                  ConsoleUtil::printSuccess("Proficiency updated.");
            ConsoleUtil::pause();

        } else if (opt == "3" && !skills.empty()) {
            const std::string numStr = ConsoleUtil::promptInput("Skill # to remove  : ");
            int idx = 0;
            try { idx = std::stoi(numStr) - 1; } catch (...) { continue; }
            if (idx < 0 || idx >= static_cast<int>(skills.size())) continue;

            const int skillId = skills[idx].value("skillId", 0);
            const std::string skillName = skills[idx].value("skillName", "");
            if (!ConsoleUtil::confirm("Remove skill \"" + skillName + "\"?")) continue;

            const auto delResp = api.del(
                "/api/admin/employees/" + idStr + "/skills/" + std::to_string(skillId),
                AppSession::get().token
            );
            if (!delResp.success) ConsoleUtil::printError(delResp.errorMessage);
            else                  ConsoleUtil::printSuccess("Skill removed.");
            ConsoleUtil::pause();
        }
    }
}

// ── Main employee menu ────────────────────────────────────────────────────────
void showAdminEmployeeMenu(const ApiClient& api) {
    while (true) {
        ConsoleUtil::clearScreen();
        ConsoleUtil::printHeader("MANAGE EMPLOYEES");

        std::cout << "1. View All Employees\n"
                  << "2. Update Employee\n"
                  << "3. Deactivate Employee\n"
                  << "4. Manage Employee Skills\n"
                  << "5. Assign Manager\n"
                  << "6. Back\n"
                  << "\nEnter option: ";

        std::string opt;
        std::getline(std::cin, opt);

        if (opt == "1") viewAllEmployees(api);
        else if (opt == "2") updateEmployee(api);
        else if (opt == "3") deactivateEmployee(api);
        else if (opt == "4") manageSkills(api);
        else if (opt == "5") assignManager(api);
        else if (opt == "6" || opt == "b" || opt == "B") return;
    }
}
