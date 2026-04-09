#pragma once
#include "../interfaces/IGeocodingService.h"
#include "../interfaces/IHttpClient.h"
#include "../interfaces/IConfigReader.h"
#include <memory>
#include <string>

class GeocodingService : public IGeocodingService {
private:
    std::shared_ptr<IHttpClient> httpClient;
    std::string apiKey;

    static const std::string BASE_URL;

    std::string buildRequestUrl(const std::string& encodedLocation);
    std::vector<GeocodingResult> parseResponse(const std::string& jsonResponse);

public:
    GeocodingService(std::shared_ptr<IHttpClient> httpClient,
                     std::shared_ptr<IConfigReader> configReader);

    std::vector<GeocodingResult> geocode(const std::string& locationName) override;
};
