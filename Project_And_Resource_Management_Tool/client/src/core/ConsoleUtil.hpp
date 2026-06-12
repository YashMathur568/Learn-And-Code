#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <utility>

#ifdef _WIN32
#  include <conio.h>
#else
#  include <termios.h>
#  include <unistd.h>
#endif

// localtime_r shim for Windows
#if defined(_WIN32) && !defined(localtime_r)
inline struct tm* localtime_r(const time_t* timep, struct tm* result) {
    localtime_s(result, timep);
    return result;
}
#endif

namespace ConsoleUtil {

// ── Box width ───────────────────────────────────────────────────────────────
constexpr int BOX_WIDTH = 52;   // inner content width (between │ chars)

// ── Clear screen ────────────────────────────────────────────────────────────
inline void clearScreen() {
    std::cout << "\033[3J\033[2J\033[H" << std::flush;
}

// ── UTF-8 display width (counts codepoints, not bytes) ──────────────────────
inline int utf8Width(const std::string& text) {
    int width = 0;
    for (size_t bytePos = 0; bytePos < text.size(); ) {
        unsigned char byte = static_cast<unsigned char>(text[bytePos]);
        if      (byte < 0x80) bytePos += 1;
        else if (byte < 0xE0) bytePos += 2;
        else if (byte < 0xF0) bytePos += 3;
        else                  bytePos += 4;
        ++width;
    }
    return width;
}

// ── Horizontal rules ────────────────────────────────────────────────────────
inline std::string hrDouble() {
    std::string line;
    for (int idx = 0; idx < BOX_WIDTH; ++idx) line += "\xE2\x95\x90"; // UTF-8 ═
    return line;
}
inline std::string hrSingle() {
    std::string line;
    for (int idx = 0; idx < BOX_WIDTH; ++idx) line += "\xE2\x94\x80"; // UTF-8 ─
    return line;
}

// ── Box drawing ─────────────────────────────────────────────────────────────
inline void printBoxTop() {
    std::cout << "╔" << hrDouble() << "╗\n";
}
inline void printBoxBottom() {
    std::cout << "╚" << hrDouble() << "╝\n";
}
inline void printBoxRow(const std::string& text) {
    int pad = BOX_WIDTH - utf8Width(text);
    if (pad < 0) pad = 0;
    std::cout << "║ " << text << std::string(static_cast<size_t>(pad - 1), ' ') << "║\n";
}
inline void printBoxDivider() {
    std::cout << "╠" << hrDouble() << "╣\n";
}

// ── Print a titled box header ────────────────────────────────────────────────
inline void printHeader(const std::string& title, const std::string& subtitle = "") {
    printBoxTop();
    printBoxRow(title);
    if (!subtitle.empty()) {
        printBoxRow(subtitle);
    }
    printBoxBottom();
    std::cout << "\n";
}

// ── Separator line ───────────────────────────────────────────────────────────
inline void printSeparator() {
    std::cout << hrSingle() << "\n";
}

// ── Prompt helpers ───────────────────────────────────────────────────────────
inline std::string promptInput(const std::string& label) {
    std::cout << label;
    std::string value;
    std::getline(std::cin, value);
    return value;
}

// Reads password without echoing characters
inline std::string promptPassword(const std::string& label) {
    std::cout << label;
    std::cout.flush();
    std::string password;
#ifdef _WIN32
    int ch;
    while ((ch = _getch()) != '\r' && ch != '\n') {
        if (ch == '\b' && !password.empty()) {
            password.pop_back();
        } else if (ch >= 32 && ch < 127) {
            password += static_cast<char>(ch);
        }
    }
    std::cout << "\n";
#else
    termios oldt{};
    tcgetattr(STDIN_FILENO, &oldt);
    termios newt = oldt;
    newt.c_lflag &= ~static_cast<unsigned int>(ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    std::getline(std::cin, password);
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    std::cout << "\n";
#endif
    return password;
}

// ── Column-padded row builder ────────────────────────────────────────────────
inline std::string col(const std::string& text, int width) {
    const int textWidth = utf8Width(text);
    if (textWidth >= width) {
        return text.substr(0, static_cast<size_t>(width - 1)) + " ";
    }
    return text + std::string(static_cast<size_t>(width - textWidth), ' ');
}

// ── Date/time helpers ────────────────────────────────────────────────────────
inline std::string currentDateTime() {
    std::time_t now = std::time(nullptr);
    std::tm     tm{};
    localtime_r(&now, &tm);
    char buf[32];
    std::strftime(buf, sizeof(buf), "%d-%m-%Y  %H:%M", &tm);
    return buf;
}

inline std::string currentDate() {
    std::time_t now = std::time(nullptr);
    std::tm     tm{};
    localtime_r(&now, &tm);
    char buf[16];
    std::strftime(buf, sizeof(buf), "%d-%m-%Y", &tm);
    return buf;
}

// Converts API date "YYYY-MM-DD" → "DD-MM-YYYY" for display
inline std::string fmtDate(const std::string& isoDate) {
    if (isoDate.size() < 10) return isoDate;
    return isoDate.substr(8, 2) + "-" + isoDate.substr(5, 2) + "-" + isoDate.substr(0, 4);
}

// Converts display date "D(D)-M(M)-YYYY" to "YYYY-MM-DD" for API.
// Returns "" if the input is not a recognisable or valid date.
inline std::string toIsoDate(const std::string& display) {
    const size_t d1 = display.find('-');
    if (d1 == std::string::npos) return "";
    const size_t d2 = display.find('-', d1 + 1);
    if (d2 == std::string::npos) return "";
    std::string day   = display.substr(0, d1);
    std::string month = display.substr(d1 + 1, d2 - d1 - 1);
    std::string year  = display.substr(d2 + 1);
    if (day.size()   == 1) day   = "0" + day;
    if (month.size() == 1) month = "0" + month;
    if (year.size() != 4 || day.size() != 2 || month.size() != 2) return "";
    try {
        const int dayNum = std::stoi(day), monthNum = std::stoi(month), yearNum = std::stoi(year);
        if (dayNum < 1 || dayNum > 31 || monthNum < 1 || monthNum > 12 || yearNum < 2000 || yearNum > 2100) return "";
    } catch (...) { return ""; }
    return year + "-" + month + "-" + day;
}

// Given any YYYY-MM-DD date, returns the Monday of that same week.
// If the input is already a Monday it is returned unchanged.
// Returns "" on invalid input.
inline std::string toWeekMonday(const std::string& isoDate) {
    if (isoDate.size() < 10) return "";
    std::tm weekTm = {};
    std::istringstream ss(isoDate);
    ss >> std::get_time(&weekTm, "%Y-%m-%d");
    if (ss.fail()) return "";
    weekTm.tm_hour = 12;
    std::mktime(&weekTm);  // fills tm_wday
    const int daysToMon = (weekTm.tm_wday == 0) ? 6 : (weekTm.tm_wday - 1);
    const std::time_t monTime = std::mktime(&weekTm) - static_cast<std::time_t>(daysToMon) * 86400;
    std::tm monTm = {};
    localtime_r(&monTime, &monTm);
    char buf[16];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d", &monTm);
    return buf;
}

// Last Monday in YYYY-MM-DD format
inline std::string lastMonday() {
    std::time_t now = std::time(nullptr);
    std::tm     nowTm{};
    localtime_r(&now, &nowTm);
    int dow = nowTm.tm_wday == 0 ? 6 : nowTm.tm_wday - 1; // Mon=0
    std::time_t monday = now - static_cast<std::time_t>(dow) * 86400;
    std::tm mondayTm{};
    localtime_r(&monday, &mondayTm);
    char buf[16];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d", &mondayTm);
    return buf;
}

// Today as YYYY-MM-DD
inline std::string todayIso() {
    std::time_t now = std::time(nullptr);
    std::tm     tm{};
    localtime_r(&now, &tm);
    char buf[16];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d", &tm);
    return buf;
}

// ── Success / error messages ─────────────────────────────────────────────────
inline void printSuccess(const std::string& msg) {
    std::cout << "\n  " << msg << " ✓\n";
}
inline void printError(const std::string& msg) {
    std::cout << "\n  ✗  Error: " << msg << "\n";
}
inline void printWarning(const std::string& msg) {
    std::cout << "  ⚠  " << msg << "\n";
}
inline void printInfo(const std::string& msg) {
    std::cout << "  " << msg << "\n";
}

// ── Pause ────────────────────────────────────────────────────────────────────
inline void pause() {
    std::cout << "\nPress Enter to continue...";
    std::string dummy;
    std::getline(std::cin, dummy);
}

// ── Confirm prompt (Y/N) ─────────────────────────────────────────────────────
inline bool confirm(const std::string& prompt) {
    std::cout << "\n" << prompt << " [Y/N]: ";
    std::string ans;
    std::getline(std::cin, ans);
    return (!ans.empty() && (ans[0] == 'y' || ans[0] == 'Y'));
}

// ── Truncate string with ellipsis ─────────────────────────────────────────────
inline std::string trunc(const std::string& s, int width) {
    if (static_cast<int>(s.size()) <= width) return s;
    if (width <= 3) return s.substr(0, static_cast<size_t>(width));
    return s.substr(0, static_cast<size_t>(width - 3)) + "...";
}

// ── Health indicator ─────────────────────────────────────────────────────────
inline std::string healthIcon(const std::string& health) {
    if (health == "ON_TRACK")  return "[OK]";
    if (health == "AT_RISK")   return "[!!]";
    if (health == "ATTENTION") return "[~~]";
    return "[?]";
}

// ── IT department and designation enumerations ───────────────────────────────
inline const std::vector<std::string> DEPARTMENTS = {
    "Engineering", "Product", "Design", "QA / Testing",
    "DevOps / Infrastructure", "Data & Analytics", "Security",
    "Architecture", "Project Management", "Management",
    "Administration", "HR & Operations", "Finance",
    "Sales & Business Development"
};

inline const std::vector<std::string> DESIGNATIONS = {
    "Intern", "Junior", "Mid-Level", "Senior",
    "Lead", "Principal", "Manager", "Senior Manager",
    "Director", "VP", "C-Level"
};

// ── Predefined IT skill catalogue ────────────────────────────────────────────
struct SkillOption { std::string name; std::string category; };

inline const std::vector<SkillOption> SKILL_OPTIONS = {
    {"C++",                    "Backend"},
    {"Java",                   "Backend"},
    {"Python",                 "Backend"},
    {"JavaScript",             "Frontend"},
    {"React / Angular / Vue",  "Frontend"},
    {"Node.js",                "Backend"},
    {"SQL / Database",         "Backend"},
    {"Docker / Kubernetes",    "DevOps"},
    {"AWS / Azure / GCP",      "DevOps"},
    {"Selenium / Playwright",  "QA"},
};

// Shows the skill catalogue and returns {name, category}.
// Pass a non-empty currentName to offer a "keep" option (returns {"",""}  on keep).
inline std::pair<std::string, std::string> selectSkill(
        const std::string& currentName = "") {
    std::cout << "\nSkills:\n";
    std::cout << "  " << col("#", 4) << col("Skill Name", 28) << "Category\n";
    for (size_t idx = 0; idx < SKILL_OPTIONS.size(); ++idx) {
        const std::string indicator =
            (SKILL_OPTIONS[idx].name == currentName) ? "  \u2190" : "";
        std::cout << "  " << col(std::to_string(idx + 1) + ".", 4)
                  << col(SKILL_OPTIONS[idx].name, 28)
                  << SKILL_OPTIONS[idx].category << indicator << "\n";
    }
    if (!currentName.empty())
        std::cout << "  0.  (keep: " << currentName << ")\n";
    const std::string sel = promptInput("Select #: ");
    if (!currentName.empty() && (sel.empty() || sel == "0"))
        return {"", ""};
    int selectedIdx = 0;
    try { selectedIdx = std::stoi(sel) - 1; } catch (...) { return {"", ""}; }
    if (selectedIdx < 0 || selectedIdx >= static_cast<int>(SKILL_OPTIONS.size()))
        return {"", ""};
    return {SKILL_OPTIONS[selectedIdx].name, SKILL_OPTIONS[selectedIdx].category};
}

// Shows a numbered list and returns the chosen value, or `current` if the user
// presses Enter / enters 0.
inline std::string selectFromList(const std::string& label,
                                   const std::vector<std::string>& options,
                                   const std::string& current) {
    std::cout << "\n" << label << ":\n";
    for (size_t idx = 0; idx < options.size(); ++idx) {
        const std::string indicator = (options[idx] == current) ? "  \u2190" : "";
        std::cout << "  " << col(std::to_string(idx + 1) + ".", 4)
                  << options[idx] << indicator << "\n";
    }
    std::cout << "  0.  (keep: " << (current.empty() ? "none" : current) << ")\n";
    const std::string sel = promptInput("Select #: ");
    if (sel.empty() || sel == "0") return current;
    int selectedIdx = 0;
    try { selectedIdx = std::stoi(sel) - 1; } catch (...) { return current; }
    if (selectedIdx < 0 || selectedIdx >= static_cast<int>(options.size())) return current;
    return options[selectedIdx];
}

} // namespace ConsoleUtil
