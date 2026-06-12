#include "EmailService.hpp"

#include <curl/curl.h>
#include <iostream>
#include <sstream>
#include <ctime>
#include <cstring>
#include <iomanip>

// ── SMTP payload reader ───────────────────────────────────────────────────────
struct PayloadState {
    const std::string* data;
    size_t             offset{0};
};

static size_t readCallback(char* ptr, size_t size, size_t nmemb, void* userp) {
    auto* state     = static_cast<PayloadState*>(userp);
    const size_t remaining = state->data->size() - state->offset;
    const size_t toCopy    = std::min(size * nmemb, remaining);
    if (toCopy == 0) return 0;
    memcpy(ptr, state->data->data() + state->offset, toCopy);
    state->offset += toCopy;
    return toCopy;
}

// ── RFC 2822 date string ──────────────────────────────────────────────────────
static std::string rfcDate() {
    std::time_t now = std::time(nullptr);
    std::tm rfcTm = {};
    localtime_s(&rfcTm, &now);
    char buf[64];
    std::strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S %z", &rfcTm);
    return buf;
}

// ── Constructor ───────────────────────────────────────────────────────────────
EmailService::EmailService(const EmailConfig& config) : config_(config) {}

// ── send() ────────────────────────────────────────────────────────────────────
bool EmailService::send(const std::string& toAddress,
                        const std::string& subject,
                        const std::string& body)
{
    if (!config_.enabled) {
        std::cout << "[Email] Disabled — would send to " << toAddress
                  << " | Subject: " << subject << "\n";
        return true;
    }

    // Build RFC 2822 message
    std::ostringstream msg;
    msg << "Date: "    << rfcDate()          << "\r\n"
        << "To: "      << toAddress          << "\r\n"
        << "From: "    << config_.fromAddress << "\r\n"
        << "Subject: " << subject            << "\r\n"
        << "MIME-Version: 1.0\r\n"
        << "Content-Type: text/plain; charset=UTF-8\r\n"
        << "\r\n"
        << body << "\r\n";
    const std::string payload = msg.str();

    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "[Email] curl_easy_init() failed\n";
        return false;
    }

    // smtp:// for STARTTLS on port 587; smtps:// for SSL on port 465
    const std::string url = "smtp://" + config_.smtpHost + ":" +
                             std::to_string(config_.smtpPort);

    struct curl_slist* recipients = nullptr;
    recipients = curl_slist_append(recipients, toAddress.c_str());

    PayloadState state{&payload, 0};

    curl_easy_setopt(curl, CURLOPT_URL,           url.c_str());
    curl_easy_setopt(curl, CURLOPT_USERNAME,      config_.username.c_str());
    curl_easy_setopt(curl, CURLOPT_PASSWORD,      config_.password.c_str());
    curl_easy_setopt(curl, CURLOPT_MAIL_FROM,     config_.fromAddress.c_str());
    curl_easy_setopt(curl, CURLOPT_MAIL_RCPT,     recipients);
    curl_easy_setopt(curl, CURLOPT_USE_SSL,       static_cast<long>(CURLUSESSL_ALL));
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
    curl_easy_setopt(curl, CURLOPT_UPLOAD,        1L);
    curl_easy_setopt(curl, CURLOPT_READFUNCTION,  readCallback);
    curl_easy_setopt(curl, CURLOPT_READDATA,      &state);
    curl_easy_setopt(curl, CURLOPT_VERBOSE,       0L);

    const CURLcode res = curl_easy_perform(curl);
    const bool ok = (res == CURLE_OK);
    if (!ok) {
        std::cerr << "[Email] Failed to send to " << toAddress
                  << ": " << curl_easy_strerror(res) << "\n";
    }

    curl_slist_free_all(recipients);
    curl_easy_cleanup(curl);
    return ok;
}
