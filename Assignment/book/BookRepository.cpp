#include "BookRepository.h"
#include <fstream>

void BookRepository::save(const Book& book)
{
    std::string filename =
        book.getTitle() + " - " + book.getAuthor() + ".txt";

    std::ofstream file(filename);
    file << "Title: " << book.getTitle() << "\n";
    file << "Author: " << book.getAuthor() << "\n";
}
