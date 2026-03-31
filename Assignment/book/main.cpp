#include <vector>
#include "Book.h"
#include "BookReader.h"
#include "PlainTextPrinter.h"

// This code follows single responsibility principle (SRP).

int main()
{
    std::vector<std::string> pages = {
        "Page 1 content",
        "Page 2 content"
    };

    Book book("A Great Book", "John Doe", pages);
    BookReader reader(book);
    PlainTextPrinter printer;

    printer.printPage(reader.getCurrentPage());
    reader.turnToNextPage();
    printer.printPage(reader.getCurrentPage());

    return 0;
}

// Why Book Violates the Single Responsibility Principle (SRP)

// 1.Multiple responsibilities in one class

// ->Represents book data (title, author)
// ->Handles reading logic (page navigation)
// ->Knows library location (shelf, room)
// ->Manages persistence (saving to file)

// 2.More than one reason to change

// ->Changes in book content → modify Book
// ->Changes in library layout → modify Book
// ->Changes in storage method (file, DB, cloud) → modify Book

// 3.Mixes domain logic with infrastructure logic

// ->Book behavior is mixed with file system operations (file_put_contents)
// ->High coupling
// ->Book is tightly coupled to file system and storage format (serialize)

// 4.Harder to test

// ->Cannot test book logic without touching file I/O

// 5.Violates separation of concerns

// ->Location and persistence are external concerns, not book responsibilities


// //code 
// class Book {
 
//     function getTitle() {
//         return "A Great Book";
//     }
 
//     function getAuthor() {
//         return "John Doe";
//     }
 
//     function turnPage() {
//         // pointer to next page
//     }
 
//     function getCurrentPage() {
//         return "current page content";
//     }
 
//     function getLocation() {
//         // returns the position in the library
//         // ie. shelf number & room number
//     }

//     function save() {
//         $filename = '/documents/'. $this->getTitle(). ' - ' . $this->getAuthor();
//         file_put_contents($filename, serialize($this));
//     }
// }

// interface Printer {
 
//     function printPage($page);
// }
 
// class PlainTextPrinter implements Printer {
 
//     function printPage($page) {
//         echo $page;
//     }
 
// }
 
// class HtmlPrinter implements Printer {
 
//     function printPage($page) {
//         echo '<div style="single-page">' . $page . '</div>';
//     }
// }
