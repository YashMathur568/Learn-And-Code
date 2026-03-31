#ifndef CUSTOMER_CSV_EXPORTER_H
#define CUSTOMER_CSV_EXPORTER_H

#include <vector>
#include <string>
#include "Customer.h"

class CustomerCsvExporter
{
public:
    static std::string exportToCsv(const std::vector<Customer>& customers);
};

#endif
