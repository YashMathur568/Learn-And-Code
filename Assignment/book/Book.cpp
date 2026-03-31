#include "Book.h"

Book::Book(std::string title, std::string author,
           const std::vector<std::string>& pages)
    : title(std::move(title)), author(std::move(author)), pages(pages)
{
}

const std::string& Book::getTitle() const
{
    return title;
}

const std::string& Book::getAuthor() const
{
    return author;
}

const std::string& Book::getPage(size_t index) const
{
    return pages.at(index);
}

size_t Book::getPageCount() const
{
    return pages.size();
}
