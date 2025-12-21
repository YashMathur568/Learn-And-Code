#include <iostream>
#include <vector>

#include "Customer.h"
#include "CustomerRepository.h"
#include "CustomerSearchService.h"
#include "CustomerCsvExporter.h"

int main()
{
    std::vector<Customer> customers = {
        {1, "ABC Corp", "John", "USA"},
        {2, "XYZ Ltd", "Amit", "India"},
        {3, "PQR Inc", "Sara", "USA"}
    };

    CustomerRepository repository(customers);
    CustomerSearchService searchService(repository);

    auto results = searchService.findByCountry("USA");

    std::string csvData = CustomerCsvExporter::exportToCsv(results);

    std::cout << csvData;

    return 0;
}
