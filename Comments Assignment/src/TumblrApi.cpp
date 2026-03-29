#include "TumblrApi.h"
#include "HttpClient.h"
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

/*
 * processBlog
 * -----------
 * Fetches blog data using Tumblr API v1 (JSON),
 * extracts required blog information and prints
 * highest resolution images for the given post range.
 */
void TumblrApi::processBlog(const std::string& blogName, int start, int end) {
    // Build HTTPS API URL (JSON format)
    std::string url = "https://" + blogName + ".tumblr.com/api/read/json";

    // Fetch raw response
    std::string rawResponse = HttpClient::get(url);

    /*
     * Tumblr API v1 returns JSON wrapped in a JS callback:
     *   var tumblr_api_read = {...};
     * We must extract only the JSON object.
     */
    size_t jsonStart = rawResponse.find('{');
    size_t jsonEnd = rawResponse.rfind('}');
    std::string jsonText = rawResponse.substr(jsonStart, jsonEnd - jsonStart + 1);

    json data = json::parse(jsonText);

    // Extract basic blog information
    auto tumblelog = data["tumblelog"];
    std::cout << "title: " << tumblelog.value("title", "") << "\n";
    std::cout << "name: " << tumblelog.value("name", "") << "\n";
    std::cout << "description: " << tumblelog.value("description", "") << "\n";
    std::cout << "no of post: " << tumblelog.value("posts", 0) << "\n\n";

    auto posts = data["posts"];
    int totalPosts = posts.size();

    // Validate user-provided range
    if (start < 1) start = 1;
    if (end > totalPosts) end = totalPosts;

    /*
     * Loop through posts in the given range.
     * Tumblr posts are zero-indexed internally.
     */
    for (int index = start - 1; index < end; ++index) {
        std::cout << (index + 1) << ". ";

        const auto& post = posts[index];

        // Only photo posts contain images
        if (post.value("type", "") == "photo") {
            bool firstImage = true;

            for (const auto& photo : post["photos"]) {
                // Highest quality image is photo-url-1280
                if (photo.contains("photo-url-1280")) {
                    if (!firstImage) {
                        std::cout << "\n   ";
                    }
                    std::cout << photo["photo-url-1280"];
                    firstImage = false;
                }
            }
        }
        std::cout << "\n";
    }
}
