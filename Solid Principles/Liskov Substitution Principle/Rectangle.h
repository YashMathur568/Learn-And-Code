#ifndef RECTANGLE_H
#define RECTANGLE_H

#include "Shape.h"

class Rectangle : public Shape
{
public:
    Rectangle(int width, int height);

    void setWidth(int width);
    void setHeight(int height);

    int getArea() const override;

protected:
    int width;
    int height;
};

#endif
