#include "CustomerCsvExporter.h"
#include <sstream>

std::string CustomerCsvExporter::exportToCsv(
    const std::vector<Customer>& customers)
{
    std::ostringstream output;

    for (const auto& customer : customers)
    {
        output << customer.customerId << ","
               << customer.companyName << ","
               << customer.contactName << ","
               << customer.country << "\n";
    }

    return output.str();
}
