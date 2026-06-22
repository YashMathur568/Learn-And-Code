#pragma once

#include <string>
#include <ctime>




inline bool isValidIsoDate(const std::string& dateStr) {
    if (dateStr.size() != 10 || dateStr[4] != '-' || dateStr[7] != '-') return false;
    int year = 0, month = 0, day = 0;
    try {
        year  = std::stoi(dateStr.substr(0, 4));
        month = std::stoi(dateStr.substr(5, 2));
        day   = std::stoi(dateStr.substr(8, 2));
    } catch (...) { return false; }

    if (year < 1900 || year > 2100 || month < 1 || month > 12 || day < 1 || day > 31) return false;

    std::tm parsedTimeStruct  = {};
    parsedTimeStruct.tm_year  = year - 1900;
    parsedTimeStruct.tm_mon   = month - 1;
    parsedTimeStruct.tm_mday  = day;
    parsedTimeStruct.tm_hour  = 12;
    std::mktime(&parsedTimeStruct);
    return (parsedTimeStruct.tm_year == year - 1900) && (parsedTimeStruct.tm_mon == month - 1) && (parsedTimeStruct.tm_mday == day);
}


inline std::string todayIso() {
    std::time_t currentEpochTime = std::time(nullptr);
    std::tm     todayTimeStruct = {};
#ifdef _WIN32
    localtime_s(&todayTimeStruct, &currentEpochTime);
#else
    localtime_r(&currentEpochTime, &todayTimeStruct);
#endif
    char dateStringFormattedBuffer[16];
    std::strftime(dateStringFormattedBuffer, sizeof(dateStringFormattedBuffer), "%Y-%m-%d", &todayTimeStruct);
    return dateStringFormattedBuffer;
}
