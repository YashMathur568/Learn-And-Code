#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <termios.h>
#include <unistd.h>
#include <ctime>
#include <iomanip>

namespace ConsoleUtil {

// ── Box width ───────────────────────────────────────────────────────────────
constexpr int BOX_WIDTH = 52;   // inner content width (between │ chars)

// ── Clear screen ────────────────────────────────────────────────────────────
inline void clearScreen() {
    std::cout << "\033[3J\033[2J\033[H" << std::flush;
}

// ── Horizontal rules ────────────────────────────────────────────────────────
inline std::string hrDouble() {
    std::string s;
    for (int i = 0; i < BOX_WIDTH; ++i) s += "\xE2\x95\x90"; // UTF-8 ═
    return s;
}
inline std::string hrSingle() {
    std::string s;
    for (int i = 0; i < BOX_WIDTH; ++i) s += "\xE2\x94\x80"; // UTF-8 ─
    return s;
}

// ── Box drawing ─────────────────────────────────────────────────────────────
inline void printBoxTop() {
    std::cout << "╔" << hrDouble() << "╗\n";
}
inline void printBoxBottom() {
    std::cout << "╚" << hrDouble() << "╝\n";
}
inline void printBoxRow(const std::string& text) {
    int pad = BOX_WIDTH - static_cast<int>(text.size());
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

    termios oldt{};
    tcgetattr(STDIN_FILENO, &oldt);
    termios newt = oldt;
    newt.c_lflag &= ~static_cast<unsigned int>(ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    std::string password;
    std::getline(std::cin, password);

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    std::cout << "\n";
    return password;
}

// ── Column-padded row builder ────────────────────────────────────────────────
inline std::string col(const std::string& text, int width) {
    if (static_cast<int>(text.size()) >= width) {
        return text.substr(0, static_cast<size_t>(width - 1)) + " ";
    }
    return text + std::string(static_cast<size_t>(width - static_cast<int>(text.size())), ' ');
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

// Converts display date "DD-MM-YYYY" → "YYYY-MM-DD" for API
inline std::string toIsoDate(const std::string& display) {
    if (display.size() < 10) return display;
    return display.substr(6, 4) + "-" + display.substr(3, 2) + "-" + display.substr(0, 2);
}

// Last Monday in YYYY-MM-DD format
inline std::string lastMonday() {
    std::time_t now = std::time(nullptr);
    std::tm     tm{};
    localtime_r(&now, &tm);
    int dow = tm.tm_wday == 0 ? 6 : tm.tm_wday - 1; // Mon=0
    std::time_t monday = now - static_cast<std::time_t>(dow) * 86400;
    std::tm mtm{};
    localtime_r(&monday, &mtm);
    char buf[16];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d", &mtm);
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

} // namespace ConsoleUtil
