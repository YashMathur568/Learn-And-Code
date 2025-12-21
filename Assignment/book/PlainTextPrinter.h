#ifndef PLAIN_TEXT_PRINTER_H
#define PLAIN_TEXT_PRINTER_H

#include "PagePrinter.h"
#include <iostream>

class PlainTextPrinter : public PagePrinter
{
public:
    void printPage(const std::string& page) const override
    {
        std::cout << page << std::endl;
    }
};

#endif
