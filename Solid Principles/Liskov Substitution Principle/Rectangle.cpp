#include "Rectangle.h"

Rectangle::Rectangle(int width, int height)
    : width(width), height(height)
{
}

void Rectangle::setWidth(int width)
{
    this->width = width;
}

void Rectangle::setHeight(int height)
{
    this->height = height;
}

int Rectangle::getArea() const
{
    return width * height;
}
