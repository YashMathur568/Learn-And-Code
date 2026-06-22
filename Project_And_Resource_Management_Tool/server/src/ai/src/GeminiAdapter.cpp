#include "GeminiAdapter.hpp"
#include "AppException.hpp"

#include <nlohmann/json.hpp>
#include <curl/curl.h>
#include <string>
#include <stdexcept>

static size_t writeCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    const size_t total = size * nmemb;
    output->append(static_cast<char*>(contents), total);
    return total;
}

GeminiAdapter::GeminiAdapter(const std::string& apiKey, const std::string& model)
    : apiKey_(apiKey)
    , model_(model) {}

std::string GeminiAdapter::generate(const std::string& prompt) {
    const std::string url =
        "https://generativelanguage.googleapis.com/v1/models/" +
        model_ + ":generateContent?key=" + apiKey_;

    const nlohmann::json requestBody = {
        {"contents", {{
            {"parts", {{{"text", prompt}}}}
        }}}
    };
    const std::string requestStr = requestBody.dump();

    CURL* curl = curl_easy_init();
    if (!curl) {
        throw AppException("Failed to initialize CURL for Gemini request.");
    }

    std::string responseBody;

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL,            url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS,     requestStr.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER,     headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,  writeCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA,      &responseBody);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT,        30L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);

    const CURLcode result = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (result != CURLE_OK) {
        throw AppException(std::string("Gemini HTTP request failed: ") + curl_easy_strerror(result));
    }

    try {
        const auto responseJson = nlohmann::json::parse(responseBody);

        if (responseJson.contains("error")) {
            const std::string errorMsg = responseJson["error"].value("message", "Unknown Gemini error.");
            throw AppException("Gemini API error: " + errorMsg);
        }

        return responseJson
            .at("candidates").at(0)
            .at("content")
            .at("parts").at(0)
            .at("text").get<std::string>();

    } catch (const AppException&) {
        throw;
    } catch (const std::exception& exception) {
        throw AppException(std::string("Failed to parse Gemini response: ") + exception.what());
    }
}
