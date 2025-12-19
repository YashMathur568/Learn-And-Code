#ifndef OFFICEPRINTER_H
#define OFFICEPRINTER_H

#include "MultiFunctionPrinter.h"

class OfficePrinter : public MultiFunctionPrinter
{
public:
    void print() override;
    void scan() override;
};

#endif
