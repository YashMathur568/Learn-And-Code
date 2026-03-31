// Why the Employee class does NOT follow SRP (Single Responsibility Principle)

// 1.Multiple responsibilities in one class

// ->Holds employee data (id, name, department)
// ->Manages business logic (terminateEmployee, isWorking)
// ->Handles persistence (saveEmployeeToDatabase)
// ->Handles presentation/reporting (XML & CSV reports)

// 2.More than one reason to change

// ->Database schema changes → modify Employee
// ->Report format changes → modify Employee
// ->Business rule changes (termination logic) → modify Employee

// 3.Mixes different layers

// ->Domain logic (employee state)
// ->Infrastructure (database saving)
// ->Presentation (report printing)

// 4.High coupling

// ->Employee depends on database and report formats

// 5.Hard to test

// ->Cannot test employee logic without database or report code

#include <iostream>
#include "Employee.h"
#include "EmploymentService.h"
#include "EmployeeRepository.h"
#include "EmployeeCsvExporter.h"
#include "EmployeeXmlExporter.h"

int main()
{
    Employee emp(101, "Yash", "Engineering");

    EmployeeRepository::save(emp);

    EmployeeCsvExporter csvExporter;
    EmployeeXmlExporter xmlExporter;

    std::cout << csvExporter.exportReport(emp) << "\n";
    std::cout << xmlExporter.exportReport(emp) << "\n";

    EmploymentService::terminateEmployment(emp);

    return 0;
}
