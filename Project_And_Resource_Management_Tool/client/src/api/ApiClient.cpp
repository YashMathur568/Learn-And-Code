#include "ApiClient.hpp"

#include <curl/curl.h>
#include <sstream>
#include <stdexcept>

static size_t apiWriteCallback(void* contents, size_t size, size_t nmemb, std::string* out) {
    const size_t total = size * nmemb;
    out->append(static_cast<char*>(contents), total);
    return total;
}

ApiClient::ApiClient(const std::string& baseUrl)
    : baseUrl_(baseUrl) {}

ApiResponse ApiClient::get(const std::string& path, const std::string& token) const {
    return perform("GET", path, "", token);
}

ApiResponse ApiClient::post(const std::string& path, const nlohmann::json& payload, const std::string& token) const {
    return perform("POST", path, payload.dump(), token);
}

ApiResponse ApiClient::put(const std::string& path, const nlohmann::json& payload, const std::string& token) const {
    return perform("PUT", path, payload.dump(), token);
}

ApiResponse ApiClient::del(const std::string& path, const std::string& token) const {
    return perform("DELETE", path, "", token);
}

ApiResponse ApiClient::perform(
    const std::string& method,
    const std::string& path,
    const std::string& bodyStr,
    const std::string& token
) const {
    ApiResponse result;

    CURL* curl = curl_easy_init();
    if (!curl) {
        result.errorMessage = "Failed to initialise CURL.";
        return result;
    }

    const std::string url = baseUrl_ + path;
    std::string       responseBody;

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "Accept: application/json");

    if (!token.empty()) {
        const std::string authHeader = "Authorization: Bearer " + token;
        headers = curl_slist_append(headers, authHeader.c_str());
    }

    curl_easy_setopt(curl, CURLOPT_URL,            url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER,     headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,  apiWriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA,      &responseBody);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT,        30L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);

    if (method == "POST") {
        curl_easy_setopt(curl, CURLOPT_POST,          1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS,    bodyStr.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, static_cast<long>(bodyStr.size()));
    } else if (method == "PUT") {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS,    bodyStr.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, static_cast<long>(bodyStr.size()));
    } else if (method == "DELETE") {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
    }

    const CURLcode curlResult = curl_easy_perform(curl);

    long httpCode = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
    result.httpCode = static_cast<int>(httpCode);

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (curlResult != CURLE_OK) {
        result.errorMessage = std::string("Network error: ") + curl_easy_strerror(curlResult);
        return result;
    }

    try {
        result.body = nlohmann::json::parse(responseBody);
    } catch (const nlohmann::json::parse_error&) {
        result.errorMessage = "Server returned invalid JSON.";
        return result;
    }

    if (result.httpCode >= 200 && result.httpCode < 300) {
        result.success = true;
    } else {
        result.success      = false;
        result.errorMessage = result.body.value("message", "Unknown server error.");
    }

    return result;
}
