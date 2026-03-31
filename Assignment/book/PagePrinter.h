#ifndef PAGE_PRINTER_H
#define PAGE_PRINTER_H

#include <string>

class PagePrinter
{
public:
    virtual ~PagePrinter() = default;
    virtual void printPage(const std::string& page) const = 0;
};

#endif
