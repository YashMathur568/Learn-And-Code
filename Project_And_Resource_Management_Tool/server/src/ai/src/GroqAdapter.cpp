#include "GroqAdapter.hpp"
#include "AppException.hpp"

#include <nlohmann/json.hpp>
#include <curl/curl.h>
#include <string>

static size_t groqWriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    const size_t total = size * nmemb;
    output->append(static_cast<char*>(contents), total);
    return total;
}

GroqAdapter::GroqAdapter(const std::string& apiKey, const std::string& model)
    : apiKey_(apiKey)
    , model_(model) {}

std::string GroqAdapter::generate(const std::string& prompt) {
    const std::string url = "https://api.groq.com/openai/v1/chat/completions";

    const nlohmann::json requestBody = {
        {"model", model_},
        {"messages", {{
            {"role", "user"},
            {"content", prompt}
        }}}
    };
    const std::string requestStr = requestBody.dump();

    CURL* curl = curl_easy_init();
    if (!curl) {
        throw AppException("Failed to initialize CURL for Groq request.");
    }

    std::string responseBody;

    const std::string authHeader = "Authorization: Bearer " + apiKey_;

    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, authHeader.c_str());

    curl_easy_setopt(curl, CURLOPT_URL,            url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS,     requestStr.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER,     headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,  groqWriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA,      &responseBody);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT,        30L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);

    const CURLcode result = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (result != CURLE_OK) {
        throw AppException(std::string("Groq HTTP request failed: ") + curl_easy_strerror(result));
    }

    try {
        const auto responseJson = nlohmann::json::parse(responseBody);

        if (responseJson.contains("error")) {
            const std::string errorMsg = responseJson["error"].value("message", "Unknown Groq error.");
            throw AppException("Groq API error: " + errorMsg);
        }

        return responseJson
            .at("choices").at(0)
            .at("message")
            .at("content").get<std::string>();

    } catch (const AppException&) {
        throw;
    } catch (const std::exception& exception) {
        throw AppException(std::string("Failed to parse Groq response: ") + exception.what());
    }
}
