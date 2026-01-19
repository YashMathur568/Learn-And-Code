#include "OrderProcessor.h"
#include <iostream>


// | Comment                               | Location          | Type                           | Why it’s Bad                        |
// | ------------------------------------- | ----------------- | ------------------------------ | ----------------------------------- |
// | // This method processes an order     | ProcessOrder      | Redundant comment              | Method name already explains this   |
// | // Check if order is null             | null check        | Redundant                      | Code is self-explanatory            |
// | // Validate the order                 | validation call   | Redundant                      | Method name explains intent         |
// | // Check inventory                    | inventory check   | Redundant                      | Clear from method call              |
// | // If no inventory, return failure    | inventory failure | Redundant                      | Code states this                    |
// | // Reserve inventory                  | reserve call      | Redundant                      | Obvious                             |
// | // Process payment                    | payment call      | Redundant                      | Method name says it                 |
// | // Check if payment succeeded         | payment result    | Redundant                      | IsSuccessful is clear               |
// | // Update inventory                   | commit            | Redundant                      | Already Clear                       |
// | // Send confirmation email            | notification      | Redundant                      | Already Clear                       |
// | // Return success                     | return            | Redundant                      | Obvious                             |
// | // Payment failed, release inventory  | else block        | Redundant                      | Code explains                       |
// | // Something went wrong               | catch block       | Noise comment                  | Adds zero info                      |
// | // Log the error                      | logging           | Redundant                      | Console.WriteLine is explicit       |
// | // Throw it                           | throw             | Redundant                      | throw; is explicit                  |
// | // TODO: Fix this later               | validation        | Zombie / TODO comment          | No context, rots over time          |
// | // Added by John on 12/15/2023...     | CancelOrder       | Journal comment                | Version control already tracks this |
// | // John says we need to refund here   | refund logic      | Misleading / Authority comment | “John” is irrelevant                |
// | // This is important!!!               | SaveOrder         | Noise / Emotional comment      | Shouting ≠ clarity                  |
// | // Gets order by ID                   | method            | Redundant                      | Method name already says it         |
// | // Saves the order                    | method            | Redundant                      | Method name already says it         |


int main()
{
    OrderValidator validator;
    InventoryService inventoryService;
    PaymentGateway paymentGateway;
    NotificationService notificationService;
    OrderRepository repository;

    OrderProcessor processor(
        validator,
        inventoryService,
        paymentGateway,
        notificationService,
        repository);

    Order order;
    order.orderId = "ORD1";
    order.customerId = "CUST1";
    order.totalAmount = 100.0;
    order.items = {"Item1", "Item2"};

    processor.processOrder(order);
    processor.cancelOrder(order.orderId);

    return 0;
}
