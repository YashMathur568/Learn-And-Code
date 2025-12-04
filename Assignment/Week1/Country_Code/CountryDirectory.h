#ifndef COUNTRY_DIRECTORY_H
#define COUNTRY_DIRECTORY_H

#include <string>
#include <map>

class CountryDirectory
{
public:
    CountryDirectory();

    std::string getCountryName(
        const std::string& countryCode) const;

    bool isValidCountryCode(
        const std::string& countryCode) const;

private:
    std::map<std::string, std::string> countryCodeToNameMap;

    void initializeDirectory();
};

#endif 
