#ifndef BOOK_READER_H
#define BOOK_READER_H

#include "Book.h"

class BookReader
{
public:
    explicit BookReader(const Book& book);

    void turnToNextPage();
    const std::string& getCurrentPage() const;

private:
    const Book& book;
    size_t currentPageIndex;
};

#endif
