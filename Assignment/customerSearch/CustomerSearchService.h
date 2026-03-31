#ifndef CUSTOMER_SEARCH_SERVICE_H
#define CUSTOMER_SEARCH_SERVICE_H

#include <vector>
#include <string>
#include "Customer.h"
#include "CustomerRepository.h"

class CustomerSearchService
{
public:
    explicit CustomerSearchService(const CustomerRepository& repository);

    std::vector<Customer> findByCountry(const std::string& country) const;
    std::vector<Customer> findByCompanyName(const std::string& companyName) const;
    std::vector<Customer> findByContactName(const std::string& contactName) const;

private:
    const CustomerRepository& repository;
};

#endif
