#ifndef EMPLOYEE_CSV_EXPORTER_H
#define EMPLOYEE_CSV_EXPORTER_H

#include "EmployeeReportExporter.h"

class EmployeeCsvExporter : public EmployeeReportExporter
{
public:
    std::string exportReport(const Employee& employee) const override;
};

#endif
