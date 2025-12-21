#ifndef BOOK_REPOSITORY_H
#define BOOK_REPOSITORY_H

#include "Book.h"
#include <string>

class BookRepository
{
public:
    static void save(const Book& book);
};

#endif
