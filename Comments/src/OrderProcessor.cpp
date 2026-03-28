#include "OrderProcessor.h"
#include <stdexcept>

OrderProcessor::OrderProcessor(
    OrderValidator& validator,
    InventoryService& inventoryService,
    PaymentGateway& paymentGateway,
    NotificationService& notificationService,
    OrderRepository& repository)
    : validator(validator),
      inventoryService(inventoryService),
      paymentGateway(paymentGateway),
      notificationService(notificationService),
      repository(repository)
{
}

bool OrderProcessor::processOrder(Order& order)
{
    if (!validator.isValid(order))
    {
        return false;
    }

    if (!inventoryService.checkAvailability(order))
    {
        return false;
    }

    inventoryService.reserveItems(order);

    try
    {
        if (!paymentGateway.processPayment(order))
        {
            inventoryService.releaseReservation(order);
            return false;
        }

        inventoryService.commitReservation(order);
        notificationService.sendOrderConfirmation(order);
        repository.save(order);
        return true;
    }
    // Inventory must always be released if payment processing fails unexpectedly
    catch (...)
    {
        inventoryService.releaseReservation(order);
        throw;
    }
}

void OrderProcessor::cancelOrder(const std::string& orderId)
{
    Order order = repository.getById(orderId);

    if (order.status == OrderStatus::Paid)
    {
        paymentGateway.refundPayment(order);
        inventoryService.restoreInventory(order);
    }

    order.status = OrderStatus::Cancelled;
    repository.save(order);
}
