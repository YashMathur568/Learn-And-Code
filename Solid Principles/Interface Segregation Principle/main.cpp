#include "SimplePrinter.h"
#include "OfficePrinter.h"

// No “fat” interface
// SimplePrinter is not forced to implement scan()
// Interfaces are role-based
// Easy to extend without breaking existing classes


int main()
{
    SimplePrinter simplePrinter;
    simplePrinter.print();

    OfficePrinter officePrinter;
    officePrinter.print();
    officePrinter.scan();

    return 0;
}
