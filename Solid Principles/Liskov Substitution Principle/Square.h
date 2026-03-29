#ifndef SQUARE_H
#define SQUARE_H

#include "Rectangle.h"

class Square : public Rectangle
{
public:
    explicit Square(int side);

    void setWidth(int side);
    void setHeight(int side);
};

#endif
