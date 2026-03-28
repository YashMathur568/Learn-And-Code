#ifndef ORDER_REPOSITORY_H
#define ORDER_REPOSITORY_H

class OrderRepository
{
public:
    void save(int orderId, double finalAmount) const;
};

#endif
