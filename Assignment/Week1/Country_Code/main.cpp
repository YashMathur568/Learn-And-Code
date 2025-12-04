#include <iostream>
#include <string>
#include "CountryDirectory.h"

bool isAlphabetOnly(const std::string& text)
{
    for (size_t characterIndex = 0; characterIndex < text.length(); ++characterIndex)
    {
        char currentCharacter = text[characterIndex];
        if (!((currentCharacter >= 'A' && currentCharacter <= 'Z') ||
              (currentCharacter >= 'a' && currentCharacter <= 'z')))
        {
            return false;
        }
    }
    return true;
}

void convertToUppercase(std::string& text)
{
    for (size_t characterIndex = 0; characterIndex < text.length(); ++characterIndex)
    {
        char currentCharacter = text[characterIndex];
        if (currentCharacter >= 'a' && currentCharacter <= 'z')
        {
            text[characterIndex] = currentCharacter - ('a' - 'A');
        }
    }
}

int main()
{
    CountryDirectory countryDirectory;
    std::string inputCountryCode;

    std::cout << "Country Lookup Program (Enter Q to quit)\n";

    while (true)
    {
        std::cout << "\nEnter Country Code (Example: IN / US / NZ): ";
        std::cin >> inputCountryCode;

        if (inputCountryCode == "Q" || inputCountryCode == "q")
        {
            std::cout << "Exiting program !" << std::endl;
            break;
        }

        if (inputCountryCode.length() != 2)
        {
            std::cout << "Error: Country code must be exactly 2 letters.";
            continue;
        }

        if (!isAlphabetOnly(inputCountryCode))
        {
            std::cout << "Error: Country code must contain only letters.";
            continue;
        }

        convertToUppercase(inputCountryCode);

        if (!countryDirectory.isValidCountryCode(inputCountryCode))
        {
            std::cout << "Error: Country code not found in directory.";
            continue;
        }

        std::string countryName =
            countryDirectory.getCountryName(inputCountryCode);

        std::cout << "Country Name: " << countryName << std::endl;
    }

    return 0;
}
