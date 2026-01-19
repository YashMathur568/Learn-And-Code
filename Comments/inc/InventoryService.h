#pragma once
#include "Order.h"

class InventoryService
{
public:
    bool checkAvailability(const Order& order);
    void reserveItems(const Order& order);
    void commitReservation(const Order& order);
    void releaseReservation(const Order& order);
    void restoreInventory(const Order& order);
};
