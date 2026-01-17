#include "OrderService.h"
#include <iostream>

void OrderService::placeOrder(int orderId, double orderAmount) const
{
    if (validator.isInvalid(orderId))
    {
        showInvalidOrderMessage();
        return;
    }

    double finalAmount = calculator.calculateFinalAmount(orderAmount);
    repository.save(orderId, finalAmount);

    showOrderSuccessMessage();
}

void OrderService::showInvalidOrderMessage() const
{
    std::cout << "Order cannot be empty" << std::endl;
}

void OrderService::showOrderSuccessMessage() const
{
    std::cout << "Order placed successfully" << std::endl;
}
