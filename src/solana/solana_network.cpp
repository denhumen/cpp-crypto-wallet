#include "solana/solana_network.hpp"
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

std::string NetworkSolana::getBalance(const std::string &base58Address) {
    CURL* curl = curl_easy_init();
    std::string response;

    json payload = {
            {"jsonrpc", "2.0"},
            {"id", 1},
            {"method", "getBalance"},
            {"params", {base58Address}}
    };

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.devnet.solana.com");
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        std::string payloadStr = payload.dump();
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payloadStr.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK)
            std::cerr << "CURL error: " << curl_easy_strerror(res) << std::endl;

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }

    return response;
}
std::string NetworkSolana::requestAirdrop(const std::string& address, int64_t lamports) {
    std::cout << "Start adding 1 SOl" << std::endl;
    CURL* curl = curl_easy_init();
    std::string response;
    json payload = {
            {"jsonrpc", "2.0"},
            {"id", 1},
            {"method", "requestAirdrop"},
            {"params", {address, lamports}}
    };

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.devnet.solana.com");
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        std::string payloadStr = payload.dump();
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payloadStr.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        CURLcode res = curl_easy_perform(curl);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        std::cout << res << " Response:" << response << std::endl;
    } else{
        std::cerr << "Not curl" << std::endl;
    };

    return response;

}