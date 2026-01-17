#ifndef ORDER_SERVICE_H
#define ORDER_SERVICE_H

#include "OrderValidator.h"
#include "OrderCalculator.h"
#include "OrderRepository.h"

class OrderService
{
public:
    void placeOrder(int orderId, double orderAmount) const;

private:
    OrderValidator validator;
    OrderCalculator calculator;
    OrderRepository repository;

    void showInvalidOrderMessage() const;
    void showOrderSuccessMessage() const;
};

#endif
