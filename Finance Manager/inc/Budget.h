#pragma once

#include <string>
#include "Category.h"

struct Budget {
    int id;
    int userId;
    Category category;
    double limitAmount;
    std::string month;
};
