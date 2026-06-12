#pragma once

#include <string>
#include <ctime>

// Returns true if s is a valid YYYY-MM-DD calendar date.
// Catches nonsense like "2024-02-30" or "2024-13-01" by letting mktime
// normalise the struct and checking whether the fields shifted.
inline bool isValidIsoDate(const std::string& s) {
    if (s.size() != 10 || s[4] != '-' || s[7] != '-') return false;
    int y = 0, m = 0, d = 0;
    try {
        y = std::stoi(s.substr(0, 4));
        m = std::stoi(s.substr(5, 2));
        d = std::stoi(s.substr(8, 2));
    } catch (...) { return false; }

    if (y < 1900 || y > 2100 || m < 1 || m > 12 || d < 1 || d > 31) return false;

    std::tm t  = {};
    t.tm_year  = y - 1900;
    t.tm_mon   = m - 1;
    t.tm_mday  = d;
    t.tm_hour  = 12;  // avoid DST midnight ambiguity
    std::mktime(&t);
    return (t.tm_year == y - 1900) && (t.tm_mon == m - 1) && (t.tm_mday == d);
}

// Returns today's date as YYYY-MM-DD.
inline std::string todayIso() {
    std::time_t now = std::time(nullptr);
    std::tm     t   = {};
#ifdef _WIN32
    localtime_s(&t, &now);
#else
    localtime_r(&now, &t);
#endif
    char buf[16];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d", &t);
    return buf;
}
