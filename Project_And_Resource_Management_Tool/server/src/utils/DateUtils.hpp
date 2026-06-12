#pragma once

#include <string>
#include <ctime>

// Returns true if s is a valid YYYY-MM-DD calendar date.
// Catches nonsense like "2024-02-30" or "2024-13-01" by letting mktime
// normalise the struct and checking whether the fields shifted.
inline bool isValidIsoDate(const std::string& dateStr) {
    if (dateStr.size() != 10 || dateStr[4] != '-' || dateStr[7] != '-') return false;
    int year = 0, month = 0, day = 0;
    try {
        year  = std::stoi(dateStr.substr(0, 4));
        month = std::stoi(dateStr.substr(5, 2));
        day   = std::stoi(dateStr.substr(8, 2));
    } catch (...) { return false; }

    if (year < 1900 || year > 2100 || month < 1 || month > 12 || day < 1 || day > 31) return false;

    std::tm tmStruct  = {};
    tmStruct.tm_year  = year - 1900;
    tmStruct.tm_mon   = month - 1;
    tmStruct.tm_mday  = day;
    tmStruct.tm_hour  = 12;  // avoid DST midnight ambiguity
    std::mktime(&tmStruct);
    return (tmStruct.tm_year == year - 1900) && (tmStruct.tm_mon == month - 1) && (tmStruct.tm_mday == day);
}

// Returns today's date as YYYY-MM-DD.
inline std::string todayIso() {
    std::time_t now = std::time(nullptr);
    std::tm     todayTm = {};
#ifdef _WIN32
    localtime_s(&todayTm, &now);
#else
    localtime_r(&now, &todayTm);
#endif
    char buf[16];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d", &todayTm);
    return buf;
}
