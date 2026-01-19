#include "OrderRepository.h"
#include <cstdio>

Order OrderRepository::getById(const std::string& orderId) const
{
    std::printf("Fetching order %s\n", orderId.c_str());
    return Order{};
}

void OrderRepository::save(const Order& order)
{
    std::printf("Saving order %s\n", order.orderId.c_str());
}
