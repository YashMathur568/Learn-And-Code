#include <iostream>
#include "CountryNeighbors.h"

bool hasExactlyTwoCharacters(const std::string& countryCode)
{
    return countryCode.length() == 2;
}

bool isAlphabet(const std::string& countryCode)
{
    for (char character : countryCode)
    {
        bool isUppercaseLetter = (character >= 'A' && character <= 'Z');
        bool isLowercaseLetter = (character >= 'a' && character <= 'z');

        if (!(isUppercaseLetter || isLowercaseLetter))
        {
            return false;
        }
    }

    return true;
}

void convertToUppercase(std::string& countryCode)
{
    for (char& character : countryCode)
    {
        if (character >= 'a' && character <= 'z')
        {
            character = static_cast<char>(character - 'a' + 'A');
        }
    }
}

bool ValidateCountryCode(
    std::string& countryCode,
    std::string& errorMessage)
{
    if (!hasExactlyTwoCharacters(countryCode))
    {
        errorMessage = "Country code must contain exactly 2 letters.";
        return false;
    }

    if (!isAlphabet(countryCode))
    {
        errorMessage = "Country code must contain alphabet letters only.";
        return false;
    }

    convertToUppercase(countryCode);
    return true;
}

int main()
{
    while (true)
    {
        std::string countryCode;

        std::cout << "\nEnter 2-letter country code (IN / US / NZ etc.) or type EXIT to quit: ";
        std::cin >> countryCode;

        if (countryCode == "EXIT" || countryCode == "exit")
        {
            std::cout << "Exiting application.\n";
            break;
        }

        std::string errorMessage;

        if (!ValidateCountryCode(countryCode, errorMessage))
        {
            std::cout << "Invalid input: " << errorMessage << std::endl;
            continue;
        }

        auto neighbors = CountryNeighbors::getNeighborCountries(countryCode);

        std::string fullName = CountryNeighbors::getCountryFullName(countryCode);

        std::cout << "\nNeighboring countries of "<< fullName << ":\n";


        if (neighbors.empty())
        {
            std::cout << " No neighbor data available\n";
            continue;
        }

        for (const auto& neighbor : neighbors)
        {
            std::cout << " - " << neighbor << std::endl;
        }
    }

    return 0;
}
