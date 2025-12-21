#ifndef CUSTOMER_REPOSITORY_H
#define CUSTOMER_REPOSITORY_H

#include <vector>
#include "Customer.h"

class CustomerRepository
{
public:
    explicit CustomerRepository(const std::vector<Customer>& customers);
    const std::vector<Customer>& getAllCustomers() const;

private:
    std::vector<Customer> customers;
};

#endif
