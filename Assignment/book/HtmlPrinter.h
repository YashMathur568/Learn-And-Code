#ifndef HTML_PRINTER_H
#define HTML_PRINTER_H

#include "PagePrinter.h"
#include <iostream>

class HtmlPrinter : public PagePrinter
{
public:
    void printPage(const std::string& page) const override
    {
        std::cout << "<div class='single-page'>"
                  << page
                  << "</div>" << std::endl;
    }
};

#endif
