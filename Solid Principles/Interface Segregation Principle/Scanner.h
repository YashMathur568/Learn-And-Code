#ifndef SCANNER_H
#define SCANNER_H

class Scanner
{
public:
    virtual ~Scanner() {}

    virtual void scan() = 0;
};

#endif
