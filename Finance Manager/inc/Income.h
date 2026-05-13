#pragma once

#include <string>

class Income {
public:
    int id;
    int userId;
    double amount;
    std::string source;
    std::string description;
    std::string date;
};
