#ifndef EMPLOYEE_XML_EXPORTER_H
#define EMPLOYEE_XML_EXPORTER_H

#include "EmployeeReportExporter.h"

class EmployeeXmlExporter : public EmployeeReportExporter
{
public:
    std::string exportReport(const Employee& employee) const override;
};

#endif
