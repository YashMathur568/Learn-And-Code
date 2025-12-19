#include "Square.h"

Square::Square(int side)
    : Rectangle(side, side)
{
}

void Square::setWidth(int side)
{
    width = side;
    height = side;
}

void Square::setHeight(int side)
{
    width = side;
    height = side;
}
