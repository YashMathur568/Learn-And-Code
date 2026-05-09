#pragma once

#include <string>

struct Income {
    int id;
    int userId;
    double amount;
    std::string source;
    std::string description;
    std::string date;
};
