#include "CountryDirectory.h"

CountryDirectory::CountryDirectory()
{
    initializeDirectory();
}

void CountryDirectory::initializeDirectory()
{
    countryCodeToNameMap["IN"] = "India";
    countryCodeToNameMap["US"] = "United States";
    countryCodeToNameMap["NZ"] = "New Zealand";
    countryCodeToNameMap["AG"] = "Afganistan";
    countryCodeToNameMap["AU"] = "Australia";
}

bool CountryDirectory::isValidCountryCode(
    const std::string& countryCode) const
{
    return countryCodeToNameMap.find(countryCode)
           != countryCodeToNameMap.end();
}

std::string CountryDirectory::getCountryName(
    const std::string& countryCode) const
{
    auto iterator = countryCodeToNameMap.find(countryCode);

    if (iterator != countryCodeToNameMap.end())
    {
        return iterator->second;
    }

    return "";
}
