#pragma once
#include "GeocodingResult.h"
#include <string>
#include <vector>

class ResultDisplay {
public:
    static void showResults(const std::string& query,
                            const std::vector<GeocodingResult>& results);
    static void showError(const std::string& errorMessage);
    static void showSeparator();
};
