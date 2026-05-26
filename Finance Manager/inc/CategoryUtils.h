#pragma once

#include <string>
#include <sstream>
#include <iomanip>
#include "Category.h"

inline std::string categoryToString(Category category) {
    switch (category) {
        case Category::FOOD:          return "FOOD";
        case Category::TRANSPORT:     return "TRANSPORT";
        case Category::UTILITIES:     return "UTILITIES";
        case Category::ENTERTAINMENT: return "ENTERTAINMENT";
        case Category::HEALTH:        return "HEALTH";
        case Category::EDUCATION:     return "EDUCATION";
        default:                      return "OTHER";
    }
}

inline Category stringToCategory(const std::string& value) {
    if (value == "FOOD")          return Category::FOOD;
    if (value == "TRANSPORT")     return Category::TRANSPORT;
    if (value == "UTILITIES")     return Category::UTILITIES;
    if (value == "ENTERTAINMENT") return Category::ENTERTAINMENT;
    if (value == "HEALTH")        return Category::HEALTH;
    if (value == "EDUCATION")     return Category::EDUCATION;
    return Category::OTHER;
}

inline std::string formatAmount(double amount) {
    std::ostringstream oss;
    oss << "₹" << std::fixed << std::setprecision(2) << amount;
    return oss.str();
}
