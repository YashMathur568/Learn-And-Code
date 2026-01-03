#ifndef TUMBLR_API_H
#define TUMBLR_API_H

#include <string>

/*
 * TumblrApi
 * ---------
 * Handles Tumblr API v1 specific logic:
 * - Building API URL
 * - Parsing JSON
 * - Printing blog info
 * - Printing image URLs
 */
class TumblrApi {
public:
    static void processBlog(const std::string& blogName, int start, int end);
};

#endif
