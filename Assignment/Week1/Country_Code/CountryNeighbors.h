#ifndef COUNTRY_NEIGHBORS_H
#define COUNTRY_NEIGHBORS_H

#include <string>
#include <vector>

class CountryNeighbors
{
public:
    static std::vector<std::string> getNeighborCountries(const std::string& countryCode);
    static std::string getCountryFullName(const std::string& countryCode);
};

#endif
