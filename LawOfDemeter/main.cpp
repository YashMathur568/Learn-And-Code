#include "Paperboy.h"
#include "Customer.h"

int main()
{
    Customer customer("Yash", "Mathur", 50.0);
    Paperboy paperboy;

    paperboy.collectPayment(customer, 20.0);

    return 0;
}
