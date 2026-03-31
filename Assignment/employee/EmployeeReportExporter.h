#ifndef EMPLOYEE_REPORT_EXPORTER_H
#define EMPLOYEE_REPORT_EXPORTER_H

#include "Employee.h"
#include <string>

class EmployeeReportExporter
{
public:
    virtual ~EmployeeReportExporter() = default;
    virtual std::string exportReport(const Employee& employee) const = 0;
};

#endif
