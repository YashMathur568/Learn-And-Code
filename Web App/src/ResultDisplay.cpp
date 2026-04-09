#include "../inc/ResultDisplay.h"
#include <iostream>
#include <iomanip>

void ResultDisplay::showResults(const std::string& query,
                                const std::vector<GeocodingResult>& results) {
    std::cout << "\n";
    showSeparator();
    std::cout << "  Search Query:   " << query << "\n";
    std::cout << "  Results Found:  " << results.size() << "\n";
    showSeparator();

    if (results.empty()) {
        std::cout << "  No results found for the given location.\n";
        showSeparator();
        return;
    }

    for (size_t index = 0; index < results.size(); ++index) {
        const auto& result = results[index];

        std::cout << "\n  Result #" << (index + 1) << ":\n";
        std::cout << "    Address:    " << result.formattedAddress << "\n";
        std::cout << "    Latitude:   " << std::fixed << std::setprecision(6)
                  << result.latitude << "\n";
        std::cout << "    Longitude:  " << std::fixed << std::setprecision(6)
                  << result.longitude << "\n";
    }

    std::cout << "\n";
    showSeparator();
}

void ResultDisplay::showError(const std::string& errorMessage) {
    std::cerr << "\n  [ERROR] " << errorMessage << "\n\n";
}

void ResultDisplay::showSeparator() {
    std::cout << "  " << std::string(50, '-') << "\n";
}
