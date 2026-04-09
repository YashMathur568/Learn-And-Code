#include "../inc/GeocodingService.h"
#include <nlohmann/json.hpp>
#include <stdexcept>

using json = nlohmann::json;

const std::string GeocodingService::BASE_URL =
    "https://maps.googleapis.com/maps/api/geocode/json";

GeocodingService::GeocodingService(std::shared_ptr<IHttpClient> httpClient,
                                   std::shared_ptr<IConfigReader> configReader)
    : httpClient(httpClient) {
    apiKey = configReader->readValue("GOOGLE_API_KEY");
    if (apiKey.empty()) {
        throw std::runtime_error("Google API key is not configured. "
                                 "Set GOOGLE_API_KEY in config.txt");
    }
}

std::string GeocodingService::buildRequestUrl(const std::string& encodedLocation) {
    return BASE_URL + "?address=" + encodedLocation + "&key=" + apiKey;
}

std::vector<GeocodingResult> GeocodingService::parseResponse(
    const std::string& jsonResponse) {

    std::vector<GeocodingResult> results;

    json response;
    try {
        response = json::parse(jsonResponse);
    } catch (const json::parse_error& parseError) {
        throw std::runtime_error("Failed to parse API response: "
                                 + std::string(parseError.what()));
    }

    std::string status = response.value("status", "UNKNOWN");

    if (status == "ZERO_RESULTS") {
        return results;
    }

    if (status != "OK") {
        std::string errorMessage = response.value("error_message",
                                                   "No details available");
        throw std::runtime_error("Geocoding API error (" + status + "): "
                                 + errorMessage);
    }

    if (!response.contains("results") || !response["results"].is_array()) {
        throw std::runtime_error("Unexpected API response format: "
                                 "missing results array");
    }

    for (const auto& resultEntry : response["results"]) {
        GeocodingResult geocodeResult;

        geocodeResult.formattedAddress =
            resultEntry.value("formatted_address", "Unknown address");

        if (!resultEntry.contains("geometry") ||
            !resultEntry["geometry"].contains("location")) {
            continue;
        }

        const auto& location = resultEntry["geometry"]["location"];
        geocodeResult.latitude = location.value("lat", 0.0);
        geocodeResult.longitude = location.value("lng", 0.0);

        results.push_back(geocodeResult);
    }

    return results;
}

std::vector<GeocodingResult> GeocodingService::geocode(
    const std::string& locationName) {

    std::string encodedLocation = httpClient->urlEncode(locationName);
    std::string requestUrl = buildRequestUrl(encodedLocation);
    std::string response = httpClient->sendGetRequest(requestUrl);

    return parseResponse(response);
}
