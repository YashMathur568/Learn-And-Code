#include "CountryNeighbors.h"

#include <unordered_map>

namespace
{
    const std::unordered_map<std::string, std::vector<std::string>> neighborMap =
    {
        { "IN", { "Pakistan", "China", "Nepal", "Bhutan", "Bangladesh", "Myanmar", "Sri Lanka" } },
        { "US", { "Canada", "Mexico" } },
        { "NZ", { "Australia" } },
        { "CA", { "United States" } },
        { "MX", { "United States", "Guatemala", "Belize" } },
        { "CN", { "India", "Pakistan", "Nepal", "Bhutan", "Myanmar", "Russia", "Mongolia" } }
    };
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
