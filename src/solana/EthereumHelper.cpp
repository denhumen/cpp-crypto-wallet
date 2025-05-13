//
// Created by denhumen on 5/13/25.
//

#include "EthereumHelper.hpp"
#include "JsonRpcHelper.hpp"
#include <nlohmann/json.hpp>
#include <iostream>
#include <curl/curl.h>

using json = nlohmann::json;

const std::string ETHER_RPC_URL = "https://api-sepolia.etherscan.io/api";
const std::string API_TOKEN = "9CFWDYF2WJNEDGAHG31UU5PK99RGJ1UINV";

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* response) {
    if (response == nullptr) return 0;
    size_t totalSize = size * nmemb;
    response->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

std::string EthereumHelper::getBalance(const std::string& address) {
    if (address.empty()) {
        return "Error: Address is empty";
    }

    std::string requestUrl = ETHER_RPC_URL
                             + "?module=account"
                             + "&action=balance"
                             + "&address=" + address
                             + "&tag=latest"
                             + "&apikey=" + API_TOKEN;

    std::string response = JsonRpcHelper::sendApiRequest(requestUrl);
    return response;
}


std::string EthereumHelper::requestAirdrop(const std::string& address, int64_t lamports) {
    return "";
    json payload = {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "requestAirdrop"},
        {"params", {address, lamports}}
    };

    std::cout << "🚀 Requesting airdrop for: " << address << " with lamports: " << lamports << std::endl;
    std::string response = JsonRpcHelper::sendJsonRpcRequest(ETHER_RPC_URL, payload);
    return response;
}
