#pragma once

#include <string>

struct Employee {
    int         employeeId{0};
    int         userId{0};
    int         managerId{0};
    std::string fullName;
    std::string email;
    std::string department;
    std::string designation;
    std::string status;
    bool        isActive{true};
};
