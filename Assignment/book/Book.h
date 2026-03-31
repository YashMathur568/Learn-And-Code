#ifndef BOOK_H
#define BOOK_H

#include <string>
#include <vector>

class Book
{
public:
    Book(std::string title, std::string author,
         const std::vector<std::string>& pages);

    const std::string& getTitle() const;
    const std::string& getAuthor() const;
    const std::string& getPage(size_t index) const;
    size_t getPageCount() const;

private:
    std::string title;
    std::string author;
    std::vector<std::string> pages;
};

#endif
