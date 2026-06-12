#pragma once
#include "../utils/ConfigLoader.hpp"
#include <string>

class EmailService {
public:
    explicit EmailService(const EmailConfig& config);

    // Returns true on success; logs and returns false on failure (never throws).
    bool send(const std::string& toAddress,
              const std::string& subject,
              const std::string& body);

private:
    EmailConfig config_;
};
