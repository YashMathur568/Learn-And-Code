#pragma once
#include <string>

struct AppSession {
    std::string token;
    std::string username;
    std::string fullName;
    std::string role;
    int         userId{0};
    bool        forcePasswordChange{false};
    bool        loggedIn{false};

    void clear() {
        token.clear();
        username.clear();
        fullName.clear();
        role.clear();
        userId              = 0;
        forcePasswordChange = false;
        loggedIn            = false;
    }

    static AppSession& get() {
        static AppSession instance;
        return instance;
    }

private:
    AppSession() = default;
};
