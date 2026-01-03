#include <iostream>
#include "TumblrApi.h"

/*
 * Entry point of the program.
 * Responsible only for:
 * - Taking user input
 * - Delegating work to TumblrApi
 */
int main() {
    std::string blogName;
    std::string range;

    std::cout << "enter the Tumblr blog name:\n";
    std::cin >> blogName;

    std::cout << "enter the range:\n";
    std::cin >> range;

    // Parse range in format start-end
    size_t dashPos = range.find('-');
    int start = std::stoi(range.substr(0, dashPos));
    int end = std::stoi(range.substr(dashPos + 1));

    try {
        TumblrApi::processBlog(blogName, start, end);
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << "\n";
    }

    return 0;
}
