#include "EmployeeCsvExporter.h"
#include <sstream>

std::string EmployeeCsvExporter::exportReport(const Employee& employee) const
{
    std::ostringstream output;
    output << employee.getId() << ","
           << employee.getName() << ","
           << employee.getDepartment();
    return output.str();
}
