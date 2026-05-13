#pragma once

#include <string>
#include "Category.h"

class Budget {
public:
    int id;
    int userId;
    Category category;
    double limitAmount;
    std::string month;
};
