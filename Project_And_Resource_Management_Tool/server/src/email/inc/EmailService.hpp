#pragma once
#include "ConfigLoader.hpp"
#include <string>

class EmailService {
public:
    explicit EmailService(const EmailConfig& config);


    bool send(const std::string& toAddress,
              const std::string& subject,
              const std::string& body);

private:
    EmailConfig config_;
};
