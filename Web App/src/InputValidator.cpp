#include "../inc/InputValidator.h"
#include <algorithm>
#include <cctype>

bool InputValidator::isValidLocationName(const std::string& locationName) {
    return getValidationError(locationName).empty();
}

std::string InputValidator::getValidationError(const std::string& locationName) {
    if (locationName.empty()) {
        return "Location name cannot be empty.";
    }

    bool isAllWhitespace = std::all_of(locationName.begin(), locationName.end(),
                                       [](unsigned char character) {
                                           return std::isspace(character);
                                       });
    if (isAllWhitespace) {
        return "Location name cannot be only whitespace.";
    }

    const size_t MAX_LOCATION_LENGTH = 200;
    if (locationName.length() > MAX_LOCATION_LENGTH) {
        return "Location name is too long (max 200 characters).";
    }

    bool hasAlphabetic = std::any_of(locationName.begin(), locationName.end(),
                                     [](unsigned char character) {
                                         return std::isalpha(character);
                                     });
    if (!hasAlphabetic) {
        return "Location name must contain at least one letter.";
    }

    return "";
}
