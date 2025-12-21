#ifndef EMPLOYEE_REPOSITORY_H
#define EMPLOYEE_REPOSITORY_H

#include "Employee.h"

class EmployeeRepository
{
public:
    static void save(const Employee& employee);
};

#endif
