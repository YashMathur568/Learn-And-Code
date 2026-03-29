#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <string>

/*
 * HttpClient
 * ----------
 * Responsible for making HTTPS GET requests.
 * This class hides libcurl details from the rest of the program.
 */
class HttpClient {
public:
    // Fetches data from the given URL and returns response as string
    static std::string get(const std::string& url);
};

#endif
