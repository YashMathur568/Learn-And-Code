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
                  << ConsoleUtil::col("Role",     10)
                  << ConsoleUtil::col("Name",     22)
                  << ConsoleUtil::col("Department", 14)
                  << "Status\n";
        ConsoleUtil::printSeparator();

        int activeCount = 0, inactiveCount = 0;
        for (const auto& emp : data) {
            if (emp.value("isActive", true)) ++activeCount; else ++inactiveCount;

            std::cout << ConsoleUtil::col(std::to_string(emp.value("userId", 0)), 6)
                      << ConsoleUtil::col(emp.value("role", ""), 10)
                      << ConsoleUtil::col(emp.value("fullName", ""), 22)
                      << ConsoleUtil::col(emp.value("department", ""), 14)
                      << emp.value("status", "") << "\n";
        }
        ConsoleUtil::printSeparator();
        std::cout << "Total: " << data.size()
                  << "   |   Active: " << activeCount
                  << "   |   Inactive: " << inactiveCount << "\n";

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

    // Fetch and display the full list so the admin can identify IDs
    const auto respAll = api.get("/api/admin/employees", AppSession::get().token);
    if (!respAll.success) { ConsoleUtil::printError(respAll.errorMessage); ConsoleUtil::pause(); return; }

    const auto& allEmps = respAll.body.value("data", nlohmann::json::array());
    std::cout << ConsoleUtil::col("ID",   6)
              << ConsoleUtil::col("Role", 10)
              << ConsoleUtil::col("Name", 24)
              << ConsoleUtil::col("Department", 22)
              << "Designation\n";
    ConsoleUtil::printSeparator();
    for (const auto& emp : allEmps) {
        std::cout << ConsoleUtil::col(std::to_string(emp.value("userId", 0)), 6)
                  << ConsoleUtil::col(emp.value("role", ""), 10)
                  << ConsoleUtil::col(emp.value("fullName", ""), 24)
                  << ConsoleUtil::col(emp.value("department", ""), 22)
                  << emp.value("designation", "") << "\n";
    }
    ConsoleUtil::printSeparator();
    std::cout << "\n";

    const std::string idStr = ConsoleUtil::promptInput("Employee ID (B to go back): ");
    if (idStr.empty() || idStr == "b" || idStr == "B") return;

    try { std::stoi(idStr); } catch (...) {
        ConsoleUtil::printError("Please enter a numeric ID.");
        ConsoleUtil::pause(); return;
    }

    nlohmann::json found;
    for (const auto& employee : allEmps) {
        if (std::to_string(employee.value("userId", 0)) == idStr) { found = employee; break; }
    }
    if (found.is_null()) { ConsoleUtil::printError("Employee not found."); ConsoleUtil::pause(); return; }

    const std::string currentName  = found.value("fullName", "");
    const std::string currentEmail = found.value("email", "");
    const std::string role         = found.value("role", "");

    ConsoleUtil::clearScreen();
    ConsoleUtil::printHeader("UPDATE EMPLOYEE");
    std::cout << "\n── " << currentName << " [" << role << "] ──\n\n";

    const std::string newFullName = ConsoleUtil::promptInput("Full Name  [" + currentName + "]: ");
    const std::string newEmail    = ConsoleUtil::promptInput("Email      [" + currentEmail + "]: ");

    const std::string resolvedName  = newFullName.empty() ? currentName : newFullName;
    const std::string resolvedEmail = newEmail.empty() ? currentEmail : newEmail;

    std::string resolvedDept  = found.value("department", "");
    std::string resolvedDesig = found.value("designation", "");

    resolvedDept  = ConsoleUtil::selectFromList("Department",  ConsoleUtil::DEPARTMENTS, resolvedDept);
    resolvedDesig = ConsoleUtil::selectFromList("Designation", ConsoleUtil::DESIGNATIONS, resolvedDesig);

    if (resolvedName  == currentName
        && resolvedEmail == currentEmail
        && resolvedDept  == found.value("department", "")
        && resolvedDesig == found.value("designation", "")) {
        ConsoleUtil::printInfo("No changes made.");
        ConsoleUtil::pause();
        return;
    }

    const nlohmann::json payload = {
        {"fullName",    resolvedName},
        {"email",       resolvedEmail},
        {"department",  resolvedDept},
        {"designation", resolvedDesig}
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

    const auto respAll = api.get("/api/admin/employees", AppSession::get().token);
    if (!respAll.success) { ConsoleUtil::printError(respAll.errorMessage); ConsoleUtil::pause(); return; }

    const auto& allEmps = respAll.body.value("data", nlohmann::json::array());
    std::cout << ConsoleUtil::col("ID",   6)
              << ConsoleUtil::col("Role", 10)
              << ConsoleUtil::col("Name", 24)
              << ConsoleUtil::col("Department", 18)
              << "Active\n";
    ConsoleUtil::printSeparator();
    for (const auto& emp : allEmps) {
        std::cout << ConsoleUtil::col(std::to_string(emp.value("userId", 0)), 6)
                  << ConsoleUtil::col(emp.value("role", ""), 10)
                  << ConsoleUtil::col(emp.value("fullName", ""), 24)
                  << ConsoleUtil::col(emp.value("department", ""), 18)
                  << (emp.value("isActive", true) ? "Yes" : "No") << "\n";
    }
    ConsoleUtil::printSeparator();
    std::cout << "\n";

    const std::string idStr = ConsoleUtil::promptInput("Employee ID (B to go back): ");
    if (idStr.empty() || idStr == "b" || idStr == "B") return;
    try { std::stoi(idStr); } catch (...) {
        ConsoleUtil::printError("Please enter a numeric ID.");
        ConsoleUtil::pause(); return;
    }

    nlohmann::json found;
    for (const auto& employee : allEmps) {
        if (std::to_string(employee.value("userId", 0)) == idStr) { found = employee; break; }
    }
    if (found.is_null()) { ConsoleUtil::printError("Employee not found."); ConsoleUtil::pause(); return; }
    if (!found.value("isActive", true)) { ConsoleUtil::printError("Employee is already inactive."); ConsoleUtil::pause(); return; }

    ConsoleUtil::clearScreen();
    ConsoleUtil::printHeader("DEACTIVATE EMPLOYEE");
    const std::string name = found.value("fullName", "");
    std::cout << "\n── " << name << " ──\n";
    std::cout << "Department : " << found.value("department", "") << "\n";
    std::cout << "Status     : " << found.value("status", "") << "\n\n";
    ConsoleUtil::printWarning("This will end all active allocations and block login.");

    if (!ConsoleUtil::confirm("Deactivate " + name + "?")) {
        ConsoleUtil::printInfo("Cancelled."); ConsoleUtil::pause(); return;
    }

    const auto resp = api.put("/api/admin/employees/" + idStr + "/deactivate", {}, AppSession::get().token);
    if (!resp.success) { ConsoleUtil::printError(resp.errorMessage); }
    else {
        ConsoleUtil::printSuccess("Employee deactivated.");
        const auto& ended = resp.body.value("endedAllocations", nlohmann::json::array());
        if (!ended.empty())
            std::cout << "  Ended allocations: " << ended.size() << "\n";
    }
    ConsoleUtil::pause();
}

// ── Reactivate employee ───────────────────────────────────────────────────────
static void reactivateEmployee(const ApiClient& api) {
    ConsoleUtil::clearScreen();
    ConsoleUtil::printHeader("REACTIVATE EMPLOYEE");

    const auto respAll = api.get("/api/admin/employees", AppSession::get().token);
    if (!respAll.success) { ConsoleUtil::printError(respAll.errorMessage); ConsoleUtil::pause(); return; }

    const auto& allEmps = respAll.body.value("data", nlohmann::json::array());
    std::cout << "Inactive employees:\n\n";
    std::cout << ConsoleUtil::col("ID",   6)
              << ConsoleUtil::col("Role", 10)
              << ConsoleUtil::col("Name", 24)
              << "Department\n";
    ConsoleUtil::printSeparator();
    int inactiveCount = 0;
    for (const auto& emp : allEmps) {
        if (emp.value("isActive", true)) continue;
        ++inactiveCount;
        std::cout << ConsoleUtil::col(std::to_string(emp.value("userId", 0)), 6)
                  << ConsoleUtil::col(emp.value("role", ""), 10)
                  << ConsoleUtil::col(emp.value("fullName", ""), 24)
                  << emp.value("department", "") << "\n";
    }
    ConsoleUtil::printSeparator();
    if (inactiveCount == 0) {
        ConsoleUtil::printInfo("No inactive employees.");
        ConsoleUtil::pause(); return;
    }
    std::cout << "\n";

    const std::string idStr = ConsoleUtil::promptInput("Employee ID (B to go back): ");
    if (idStr.empty() || idStr == "b" || idStr == "B") return;
    try { std::stoi(idStr); } catch (...) {
        ConsoleUtil::printError("Please enter a numeric ID.");
        ConsoleUtil::pause(); return;
    }

    nlohmann::json found;
    for (const auto& emp : allEmps) {
        if (std::to_string(emp.value("userId", 0)) == idStr) { found = emp; break; }
    }
    if (found.is_null()) { ConsoleUtil::printError("Employee not found."); ConsoleUtil::pause(); return; }
    if (found.value("isActive", false)) { ConsoleUtil::printError("Employee is already active."); ConsoleUtil::pause(); return; }

    if (!ConsoleUtil::confirm("Reactivate " + found.value("fullName", "") + "?")) {
        ConsoleUtil::printInfo("Cancelled."); ConsoleUtil::pause(); return;
    }

    const auto resp = api.put("/api/admin/users/" + idStr + "/reactivate", {}, AppSession::get().token);
    if (!resp.success) { ConsoleUtil::printError(resp.errorMessage); }
    else               { ConsoleUtil::printSuccess("Employee reactivated. They can now log in."); }
    ConsoleUtil::pause();
}

// ── Assign manager ────────────────────────────────────────────────────────────
static void assignManager(const ApiClient& api) {
    ConsoleUtil::clearScreen();
    ConsoleUtil::printHeader("ASSIGN MANAGER");

    const auto respAll = api.get("/api/admin/employees", AppSession::get().token);
    if (!respAll.success) { ConsoleUtil::printError(respAll.errorMessage); ConsoleUtil::pause(); return; }
    const auto& allEmps = respAll.body.value("data", nlohmann::json::array());

    std::cout << "Managers:\n";
    std::cout << ConsoleUtil::col("ID",   6) << "Name\n";
    ConsoleUtil::printSeparator();
    for (const auto& emp : allEmps) {
        if (emp.value("role", "") != "MANAGER") continue;
        std::cout << ConsoleUtil::col(std::to_string(emp.value("userId", 0)), 6)
                  << emp.value("fullName", "") << "\n";
    }
    ConsoleUtil::printSeparator();
    std::cout << "\nEmployees:\n";
    std::cout << ConsoleUtil::col("ID",   6)
              << ConsoleUtil::col("Name", 24)
              << "Current Manager\n";
    ConsoleUtil::printSeparator();
    for (const auto& emp : allEmps) {
        if (emp.value("role", "") == "MANAGER") continue;
        std::cout << ConsoleUtil::col(std::to_string(emp.value("userId", 0)), 6)
                  << ConsoleUtil::col(emp.value("fullName", ""), 24)
                  << emp.value("managerName", "(none)") << "\n";
    }
    ConsoleUtil::printSeparator();
    std::cout << "\n";

    const std::string empId = ConsoleUtil::promptInput("Employee ID  (B to go back): ");
    if (empId.empty() || empId == "b" || empId == "B") return;
    const std::string mgrId = ConsoleUtil::promptInput("Manager ID   : ");
    if (mgrId.empty()) { ConsoleUtil::printError("Manager ID required."); ConsoleUtil::pause(); return; }

    const auto resp = api.put(
        "/api/admin/employees/" + empId + "/assign-manager",
        {{"managerId", std::stoi(mgrId)}},
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

    const auto respAll = api.get("/api/admin/employees", AppSession::get().token);
    if (!respAll.success) { ConsoleUtil::printError(respAll.errorMessage); ConsoleUtil::pause(); return; }
    const auto& allEmps = respAll.body.value("data", nlohmann::json::array());
    std::cout << ConsoleUtil::col("ID",   6)
              << ConsoleUtil::col("Role", 10)
              << ConsoleUtil::col("Name", 24)
              << "Department\n";
    ConsoleUtil::printSeparator();
    for (const auto& emp : allEmps) {
        std::cout << ConsoleUtil::col(std::to_string(emp.value("userId", 0)), 6)
                  << ConsoleUtil::col(emp.value("role", ""), 10)
                  << ConsoleUtil::col(emp.value("fullName", ""), 24)
                  << emp.value("department", "") << "\n";
    }
    ConsoleUtil::printSeparator();
    std::cout << "\n";

    const std::string idStr = ConsoleUtil::promptInput("Employee ID (B to go back): ");
    if (idStr.empty() || idStr == "b" || idStr == "B") return;
    try { std::stoi(idStr); } catch (...) { ConsoleUtil::printError("Please enter a numeric ID."); ConsoleUtil::pause(); return; }

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
            int skillIndex = 1;
            std::cout << ConsoleUtil::col("#", 4)
                      << ConsoleUtil::col("Skill", 24)
                      << ConsoleUtil::col("Category", 14)
                      << "Proficiency\n";
            ConsoleUtil::printSeparator();
            for (const auto& skill : skills) {
                std::cout << ConsoleUtil::col(std::to_string(skillIndex++), 4)
                          << ConsoleUtil::col(skill.value("skillName", ""), 24)
                          << ConsoleUtil::col(skill.value("category", ""), 14)
                          << skill.value("proficiency", "") << "\n";
            }
        }

        std::cout << "\n1. Add Skill\n2. Update Skill\n3. Remove Skill\n4. Back\n\nOption: ";
        std::string opt;
        std::getline(std::cin, opt);

        if (opt == "4" || opt == "b" || opt == "B") return;

        if (opt == "1") {
            ConsoleUtil::clearScreen();
            ConsoleUtil::printHeader("ADD SKILL");
            const auto [skillName, category] = ConsoleUtil::selectSkill();
            if (skillName.empty()) continue;

            std::cout << "\nProficiency:\n";
            std::cout << "  1.  Beginner\n";
            std::cout << "  2.  Intermediate\n";
            std::cout << "  3.  Advanced\n";
            const std::string profOpt = ConsoleUtil::promptInput("Select #: ");
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

            const int skillId          = skills[idx].value("skillId", 0);
            const std::string currentSkillName = skills[idx].value("skillName", "");
            const std::string currentCategory  = skills[idx].value("category", "Other");

            ConsoleUtil::clearScreen();
            ConsoleUtil::printHeader("UPDATE SKILL");
            const auto [newSkillName, newCategory] = ConsoleUtil::selectSkill(currentSkillName);
            const std::string resolvedName     = newSkillName.empty() ? currentSkillName : newSkillName;
            const std::string resolvedCategory = newSkillName.empty() ? currentCategory  : newCategory;

            std::cout << "\nProficiency:\n";
            std::cout << "  1.  Beginner\n";
            std::cout << "  2.  Intermediate\n";
            std::cout << "  3.  Advanced\n";
            std::cout << "  0.  (keep: " << skills[idx].value("proficiency", "") << ")\n";
            const std::string profOpt = ConsoleUtil::promptInput("Select #: ");
            const std::vector<std::string> profs = {"Beginner","Intermediate","Advanced"};
            std::string proficiency = skills[idx].value("proficiency", "Beginner");
            if (profOpt >= "1" && profOpt <= "3") proficiency = profs[std::stoi(profOpt) - 1];

            const auto updResp = api.put(
                "/api/admin/employees/" + idStr + "/skills/" + std::to_string(skillId),
                {{"skillName", resolvedName}, {"category", resolvedCategory}, {"proficiency", proficiency}},
                AppSession::get().token
            );
            if (!updResp.success) ConsoleUtil::printError(updResp.errorMessage);
            else                  ConsoleUtil::printSuccess("Skill updated.");
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
                  << "4. Reactivate Employee\n"
                  << "5. Manage Employee Skills\n"
                  << "6. Assign Manager\n"
                  << "7. Back\n"
                  << "\nEnter option: ";

        std::string opt;
        std::getline(std::cin, opt);

        if (opt == "1") viewAllEmployees(api);
        else if (opt == "2") updateEmployee(api);
        else if (opt == "3") deactivateEmployee(api);
        else if (opt == "4") reactivateEmployee(api);
        else if (opt == "5") manageSkills(api);
        else if (opt == "6") assignManager(api);
        else if (opt == "7" || opt == "b" || opt == "B") return;
    }
}
