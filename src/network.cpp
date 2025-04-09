//
// Created by denhumen on 4/9/25.
//

#include "network.h"
#include <iostream>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

std::string Network::fetchAddressInfo(const std::string& address) {
    CURL* curl = curl_easy_init();
    std::string response;

    if (curl) {
        std::string url = "https://blockstream.info/testnet/api/address/" + address;

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_USERAGENT, "cpp-crypto-wallet/1.0");

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "CURL error: " << curl_easy_strerror(res) << std::endl;
        }

        curl_easy_cleanup(curl);
    }

    return response;
}

void Network::printBalanceFromResponse(const std::string& jsonResponse) {
    try {
        json j = json::parse(jsonResponse);
        auto funded = j["chain_stats"]["funded_txo_sum"].get<int64_t>();
        auto spent = j["chain_stats"]["spent_txo_sum"].get<int64_t>();
        auto tx_count = j["chain_stats"]["tx_count"].get<int>();

        double balanceBTC = (funded - spent) / 100000000.0;

        std::cout << "Balance: " << balanceBTC << " BTC" << std::endl;
        std::cout << "Transaction count: " << tx_count << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error parsing JSON: " << e.what() << std::endl;
    }
}
