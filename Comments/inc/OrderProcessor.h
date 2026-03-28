#pragma once
#include "OrderValidator.h"
#include "InventoryService.h"
#include "PaymentGateway.h"
#include "NotificationService.h"
#include "OrderRepository.h"

class OrderProcessor
{
public:
    OrderProcessor(
        OrderValidator& validator,
        InventoryService& inventoryService,
        PaymentGateway& paymentGateway,
        NotificationService& notificationService,
        OrderRepository& repository);

    bool processOrder(Order& order);
    void cancelOrder(const std::string& orderId);

private:
    OrderValidator& validator;
    InventoryService& inventoryService;
    PaymentGateway& paymentGateway;
    NotificationService& notificationService;
    OrderRepository& repository;
};
