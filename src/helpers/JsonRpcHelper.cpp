//
// Created by denhumen on 5/13/25.
//

#include "JsonRpcHelper.hpp"
#include <curl/curl.h>
#include <iostream>
#include <mutex>

using json = nlohmann::json;

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

std::string JsonRpcHelper::sendJsonRpcRequest(const std::string& url, const json& payload) {
    CURL* curl = curl_easy_init();
    std::string response;

    if (curl) {
        std::string payloadStr = payload.dump();
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payloadStr.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "CURL error: " << curl_easy_strerror(res) << std::endl;
        }

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }

    return response;
}

std::string JsonRpcHelper::sendApiRequest(const std::string& url, const std::string& api_token) {
    CURL* curl = curl_easy_init();
    std::string response;

    if (curl) {
        if (!api_token.empty()) {
            std::cout << "use auth: "<< api_token<< std::endl;
            struct curl_slist* headers = nullptr;
            headers = curl_slist_append(headers,
                ("Authorization: Bearer " + api_token).c_str());
            headers = curl_slist_append(headers, "Accept: application/json");

            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        }

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "CURL API Error: " << curl_easy_strerror(res) << std::endl;
        }

        curl_easy_cleanup(curl);
    } else {
        response = "Error: CURL initialization failed.";
    }

    return response;
}

std::string JsonRpcHelper::getOAuthToken(
        const std::string& clientId,
        const std::string& clientSecret,
        const std::string& tokenUrl
) {
    static std::string cachedToken;
    static std::mutex mtx;
    std::lock_guard<std::mutex> lock(mtx);
    if (!cachedToken.empty()) return cachedToken;

    CURL* curl = curl_easy_init();
    std::string resp;
    if (curl) {
        char* escClientId = curl_easy_escape(curl, clientId.c_str(), 0);
        char* escSecret   = curl_easy_escape(curl, clientSecret.c_str(), 0);

        std::string body = std::string("client_id=")     + escClientId +
                           "&client_secret=" + escSecret +
                           "&grant_type=client_credentials"
                           "&scope=openid";


        curl_free(escClientId);
        curl_free(escSecret);

        struct curl_slist* hdrs = nullptr;
        hdrs = curl_slist_append(hdrs, "Content-Type: application/x-www-form-urlencoded");
        curl_easy_setopt(curl, CURLOPT_URL, tokenUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, hdrs);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp);

        if (curl_easy_perform(curl) != CURLE_OK) {
            std::cerr << "OAuth CURL error: "
                      << curl_easy_strerror(curl_easy_perform(curl)) << "\n";
        }

        curl_slist_free_all(hdrs);
        curl_easy_cleanup(curl);
    }

    try {
        auto j = json::parse(resp);
        if (j.contains("access_token")) {
            cachedToken = j["access_token"].get<std::string>();
        } else {
            std::cerr << "No access_token in response: " << resp << "\n";
        }
    } catch (...) {
        std::cerr << "Failed to parse OAuth response: " << resp << "\n";
    }
    return cachedToken;
}
