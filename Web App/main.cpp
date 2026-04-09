#include "inc/HttpClient.h"
#include "inc/ConfigReader.h"
#include "inc/GeocodingService.h"
#include "inc/InputValidator.h"
#include "inc/ResultDisplay.h"
#include <iostream>
#include <memory>
#include <string>

int main() {
    try {
        auto configReader = std::make_shared<ConfigReader>("config.txt");
        auto httpClient = std::make_shared<HttpClient>();
        auto geocodingService = std::make_shared<GeocodingService>(
            httpClient, configReader);

        std::cout << "\n  ========================================\n";
        std::cout << "       Google Geocoding Lookup Tool\n";
        std::cout << "  ========================================\n\n";

        std::string userInput;

        while (true) {
            std::cout << "  Enter a location (or 'quit' to exit): ";
            std::getline(std::cin, userInput);

            if (userInput == "quit" || userInput == "exit") {
                std::cout << "\n  Goodbye!\n";
                break;
            }

            std::string validationError =
                InputValidator::getValidationError(userInput);

            if (!validationError.empty()) {
                ResultDisplay::showError(validationError);
                continue;
            }

            try {
                auto results = geocodingService->geocode(userInput);
                ResultDisplay::showResults(userInput, results);
            } catch (const std::exception& error) {
                ResultDisplay::showError(error.what());
            }
        }

    } catch (const std::exception& error) {
        std::cerr << "\n  [FATAL] " << error.what() << "\n";
        return 1;
    }

    return 0;
}
