#ifndef PRINTER_H
#define PRINTER_H

class Printer
{
public:
    virtual ~Printer() {}

    virtual void print() = 0;
};

#endif
