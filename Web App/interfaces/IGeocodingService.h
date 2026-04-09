#pragma once
#include <string>
#include <vector>
#include "../inc/GeocodingResult.h"

class IGeocodingService {
public:
    virtual ~IGeocodingService() = default;
    virtual std::vector<GeocodingResult> geocode(const std::string& locationName) = 0;
};
