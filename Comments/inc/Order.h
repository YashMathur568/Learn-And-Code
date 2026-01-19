#pragma once
#include <vector>
#include <string>

enum class OrderStatus
{
    Created,
    Paid,
    Cancelled
};

class Order
{
public:
    std::string orderId;
    std::string customerId;
    double totalAmount{};
    std::vector<std::string> items;
    OrderStatus status{OrderStatus::Created};
    std::string transactionId;
};
