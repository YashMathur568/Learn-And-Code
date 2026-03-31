#include "BookReader.h"

BookReader::BookReader(const Book& book)
    : book(book), currentPageIndex(0)
{
}

void BookReader::turnToNextPage()
{
    if (currentPageIndex + 1 < book.getPageCount())
    {
        ++currentPageIndex;
    }
}

const std::string& BookReader::getCurrentPage() const
{
    return book.getPage(currentPageIndex);
}
