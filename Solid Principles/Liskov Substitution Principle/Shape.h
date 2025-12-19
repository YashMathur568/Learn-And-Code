#ifndef SHAPE_H
#define SHAPE_H

class Shape
{
public:
    virtual ~Shape() {}

    virtual int getArea() const = 0;
};

#endif
