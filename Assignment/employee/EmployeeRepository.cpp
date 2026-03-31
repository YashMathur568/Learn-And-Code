#include "EmployeeRepository.h"
#include <iostream>

void EmployeeRepository::save(const Employee& employee)
{
    std::cout << "Saving employee " << employee.getId()
              << " to database\n";
}
