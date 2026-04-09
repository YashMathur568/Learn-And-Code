#include "../inc/HttpClient.h"
#include <curl/curl.h>
#include <stdexcept>

static size_t writeCallback(void* contents, size_t size, size_t memberCount,
                            std::string* output) {
    size_t totalBytes = size * memberCount;
    output->append(static_cast<char*>(contents), totalBytes);
    return totalBytes;
}

HttpClient::HttpClient() {
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

HttpClient::~HttpClient() {
    curl_global_cleanup();
}

std::string HttpClient::sendGetRequest(const std::string& url) {
    CURL* curlHandle = curl_easy_init();
    if (!curlHandle) {
        throw std::runtime_error("Failed to initialize CURL session");
    }

    std::string responseBody;

    curl_easy_setopt(curlHandle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, &responseBody);
    curl_easy_setopt(curlHandle, CURLOPT_TIMEOUT, 10L);
    curl_easy_setopt(curlHandle, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curlHandle, CURLOPT_SSL_VERIFYPEER, 1L);

    CURLcode resultCode = curl_easy_perform(curlHandle);

    if (resultCode != CURLE_OK) {
        std::string errorDetail = curl_easy_strerror(resultCode);
        curl_easy_cleanup(curlHandle);
        throw std::runtime_error("HTTP request failed: " + errorDetail);
    }

    long httpStatusCode = 0;
    curl_easy_getinfo(curlHandle, CURLINFO_RESPONSE_CODE, &httpStatusCode);
    curl_easy_cleanup(curlHandle);

    if (httpStatusCode != 200) {
        throw std::runtime_error("HTTP error: received status code "
                                 + std::to_string(httpStatusCode));
    }

    return responseBody;
}

std::string HttpClient::urlEncode(const std::string& value) {
    CURL* curlHandle = curl_easy_init();
    if (!curlHandle) {
        throw std::runtime_error("Failed to initialize CURL for URL encoding");
    }

    char* encodedValue = curl_easy_escape(curlHandle, value.c_str(),
                                          static_cast<int>(value.length()));
    if (!encodedValue) {
        curl_easy_cleanup(curlHandle);
        throw std::runtime_error("URL encoding failed");
    }

    std::string encodedString(encodedValue);
    curl_free(encodedValue);
    curl_easy_cleanup(curlHandle);

    return encodedString;
}
