#pragma once
#include "Order.h"
#include <string>

class OrderRepository
{
public:
    Order getById(const std::string& orderId) const;
    void save(const Order& order);
};
