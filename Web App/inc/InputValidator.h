#pragma once
#include <string>

class InputValidator {
public:
    static bool isValidLocationName(const std::string& locationName);
    static std::string getValidationError(const std::string& locationName);
};
