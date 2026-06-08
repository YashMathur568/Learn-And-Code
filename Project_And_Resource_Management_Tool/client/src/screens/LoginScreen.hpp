#pragma once
#include "../api/ApiClient.hpp"

// Returns true if login succeeded and user is ready for their menu.
// Handles force-password-change loop internally.
bool showLoginScreen(const ApiClient& api);
