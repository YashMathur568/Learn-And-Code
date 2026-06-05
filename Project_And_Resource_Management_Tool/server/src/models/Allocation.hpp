#pragma once

#include <string>

struct Allocation {
    int         allocationId{0};
    int         employeeId{0};
    int         projectId{0};
    int         utilisation{0};
    std::string fromDate;
    std::string toDate;
    bool        isActive{true};
};
