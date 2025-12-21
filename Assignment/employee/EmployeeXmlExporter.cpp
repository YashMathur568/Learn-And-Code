#include "EmployeeXmlExporter.h"
#include <sstream>

std::string EmployeeXmlExporter::exportReport(const Employee& employee) const
{
    std::ostringstream output;
    output << "<employee>\n"
           << "  <id>" << employee.getId() << "</id>\n"
           << "  <name>" << employee.getName() << "</name>\n"
           << "  <department>" << employee.getDepartment() << "</department>\n"
           << "</employee>";
    return output.str();
}
