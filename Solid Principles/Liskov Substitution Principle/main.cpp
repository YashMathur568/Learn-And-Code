#include <iostream>
#include "Rectangle.h"
#include "Square.h"

//In this design, Square correctly maintains its invariants while still being substitutable for Rectangle and Shape. Any code written against the base class continues to work without modification

void printArea(const Shape& shape)
{
    std::cout << "Area: " << shape.getArea() << std::endl;
}

int main()
{
    Rectangle rectangle(10, 5);
    Square square(5);

    printArea(rectangle);
    printArea(square);

    return 0;
}
