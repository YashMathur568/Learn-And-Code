#include "CountryNeighbors.h"

#include <unordered_map>

const std::unordered_map<std::string, std::string> countryNameMap =
{
    {"IN", "India"},
    {"US", "United States"},
    {"CA", "Canada"},
    {"MX", "Mexico"},
    {"CN", "China"},
    {"RU", "Russia"},
    {"KR", "South Korea"},
    {"KP", "North Korea"},
    {"PK", "Pakistan"},
    {"AF", "Afghanistan"},
    {"IR", "Iran"},
    {"TR", "Turkey"},
    {"FR", "France"},
    {"DE", "Germany"},
    {"IT", "Italy"},
    {"ES", "Spain"},
    {"BR", "Brazil"},
    {"AR", "Argentina"},
    {"NZ", "New Zealand"},
    {"EG", "Egypt"},
    {"SA", "Saudi Arabia"},
    {"ZA", "South Africa"}
};


const std::unordered_map<std::string, std::vector<std::string>> neighborMap =
{
    { "IN", { "Pakistan", "China", "Nepal", "Bhutan", "Bangladesh", "Myanmar", "Sri Lanka" } },
    { "US", { "Canada", "Mexico" } },
    { "CA", { "United States" } },
    { "MX", { "United States", "Guatemala", "Belize" } },
    { "CN", { "India", "Pakistan", "Nepal", "Bhutan", "Myanmar","Russia", "Mongolia", "Kazakhstan", "Laos", "Vietnam", "North Korea" } },
    { "RU", { "Norway", "Finland", "Estonia", "Latvia", "Lithuania","Poland", "Ukraine", "Georgia", "Kazakhstan", "China", "Mongolia", "North Korea" } },
    { "KR", { "North Korea" } },
    { "KP", { "South Korea", "China", "Russia" } },
    { "PK", { "India", "China", "Afghanistan", "Iran" } },
    { "AF", { "Pakistan", "Iran", "Turkmenistan", "Uzbekistan", "Tajikistan", "China" } },
    { "IR", { "Pakistan", "Afghanistan", "Turkey", "Iraq", "Armenia", "Azerbaijan", "Turkmenistan" } },
    { "TR", { "Greece", "Bulgaria", "Georgia", "Armenia", "Azerbaijan", "Iran", "Iraq", "Syria" } },
    { "FR", { "Germany", "Belgium", "Luxembourg", "Switzerland","Italy", "Spain", "Monaco" } },
    { "DE", { "France", "Belgium", "Netherlands", "Luxembourg","Switzerland", "Austria", "Czech Republic", "Poland", "Denmark" } },
    { "IT", { "France", "Switzerland", "Austria", "Slovenia", "Vatican City", "San Marino" } },
    { "ES", { "France", "Portugal", "Andorra", "Gibraltar" } },
    { "BR", { "Argentina", "Uruguay", "Paraguay", "Bolivia","Peru", "Colombia", "Venezuela", "Guyana", "Suriname", "French Guiana" } },
    { "AR", { "Chile", "Bolivia", "Paraguay", "Brazil", "Uruguay" } },  
    { "NZ", { "Australia" } },
    { "EG", { "Libya", "Sudan", "Israel", "Gaza Strip" } },
    { "SA", { "Jordan", "Iraq", "Kuwait", "Qatar","United Arab Emirates", "Oman", "Yemen" } },
    { "ZA", { "Namibia", "Botswana", "Zimbabwe","Mozambique", "Eswatini", "Lesotho" } }
};

std::string CountryNeighbors::getCountryFullName(const std::string& countryCode)
{
    auto iterator = countryNameMap.find(countryCode);

    if (iterator != countryNameMap.end())
    {
        return iterator->second;
    }

    return countryCode;
}

std::vector<std::string> CountryNeighbors::getNeighborCountries(const std::string& countryCode)
{
    auto iterator = neighborMap.find(countryCode);

    if (iterator != neighborMap.end())
    {
        return iterator->second;
    }

    return {};
}
