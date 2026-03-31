#include "CustomerRepository.h"

CustomerRepository::CustomerRepository(const std::vector<Customer>& customers)
    : customers(customers)
{
}

const std::vector<Customer>& CustomerRepository::getAllCustomers() const
{
    return customers;
}
