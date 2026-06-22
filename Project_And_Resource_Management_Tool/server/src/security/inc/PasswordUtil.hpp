#pragma once

#include <string>

class PasswordUtil {
public:
    static std::string hash(const std::string& plainPassword);
    static bool        verify(const std::string& plainPassword, const std::string& storedHash);
    static bool        meetsStrengthPolicy(const std::string& password);

private:
    PasswordUtil() = delete;
};
