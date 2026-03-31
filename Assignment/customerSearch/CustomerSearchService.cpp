#include "CustomerSearchService.h"

std::vector<Customer> CustomerSearchService::findByCountry(
    const std::string& country) const
{
    std::vector<Customer> result;

    for (const auto& customer : repository.getAllCustomers())
    {
        if (customer.country.find(country) != std::string::npos)
        {
            result.push_back(customer);
        }
    }

    return result;
}

std::vector<Customer> CustomerSearchService::findByCompanyName(
    const std::string& companyName) const
{
    std::vector<Customer> result;

    for (const auto& customer : repository.getAllCustomers())
    {
        if (customer.companyName.find(companyName) != std::string::npos)
        {
            result.push_back(customer);
        }
    }

    return result;
}

std::vector<Customer> CustomerSearchService::findByContactName(
    const std::string& contactName) const
{
    std::vector<Customer> result;

    for (const auto& customer : repository.getAllCustomers())
    {
        if (customer.contactName.find(contactName) != std::string::npos)
        {
            result.push_back(customer);
        }
    }

    return result;
}
