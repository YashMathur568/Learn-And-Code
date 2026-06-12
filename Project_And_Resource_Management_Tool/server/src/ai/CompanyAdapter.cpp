#include "CompanyAdapter.hpp"
#include "../utils/AppException.hpp"

#include <nlohmann/json.hpp>
#include <curl/curl.h>
#include <string>

static size_t companyWriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    output->append(static_cast<char*>(contents), size * nmemb);
    return size * nmemb;
}

CompanyAdapter::CompanyAdapter(
    const std::string& host,
    const std::string& apiKey,
    const std::string& model)
    : host_(host)
    , apiKey_(apiKey)
    , model_(model) {}

std::string CompanyAdapter::generate(const std::string& prompt) {
    const std::string url = host_ + "/api/generate";

    const nlohmann::json requestBody = {
        {"model",  model_},
        {"prompt", prompt},
        {"stream", false}
    };
    const std::string requestStr = requestBody.dump();

    CURL* curl = curl_easy_init();
    if (!curl)
        throw AppException("Failed to initialize CURL for company LLM request.");

    std::string responseBody;

    const std::string apikeyHeader = "apikey: " + apiKey_;
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, apikeyHeader.c_str());

    curl_easy_setopt(curl, CURLOPT_URL,            url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS,     requestStr.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER,     headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,  companyWriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA,      &responseBody);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT,        120L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // internal HTTP endpoint

    const CURLcode result = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (result != CURLE_OK)
        throw AppException(std::string("Company LLM request failed: ") + curl_easy_strerror(result));

    try {
        const auto resp = nlohmann::json::parse(responseBody);

        if (resp.contains("error"))
            throw AppException("Company LLM error: " + resp["error"].get<std::string>());

        return resp.at("response").get<std::string>();

    } catch (const AppException&) {
        throw;
    } catch (const std::exception& ex) {
        throw AppException(std::string("Failed to parse company LLM response: ") + ex.what());
    }
}
