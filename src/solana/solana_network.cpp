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

std::string NetworkSolana::sendJsonRpcRequest(const json& payload) {
    CURL* curl = curl_easy_init();
    std::string response;

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
        if (res != CURLE_OK) {
            std::cerr << "CURL error: " << curl_easy_strerror(res) << std::endl;
        }
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
    }

    return response;
}


std::string NetworkSolana::getBalance(const std::string &base58Address) {

    json payload = {
            {"jsonrpc", "2.0"},
            {"id", 1},
            {"method", "getBalance"},
            {"params", {base58Address}}
    };

    return sendJsonRpcRequest(payload);
}
std::string NetworkSolana::requestAirdrop(const std::string& address, int64_t lamports) {
    std::cout << "Start adding 1 SOl" << std::endl;

    json payload = {
            {"jsonrpc", "2.0"},
            {"id", 1},
            {"method", "requestAirdrop"},
            {"params", {address, lamports}}
    };


    return sendJsonRpcRequest(payload);

}

std::string NetworkSolana::simulateTransaction(const std::string& base64Transaction) {

    json payload = {
            {"jsonrpc", "2.0"},
            {"id", 1},
            {"method", "simulateTransaction"},
            {"params", json::array({ base64Transaction, { {"sigVerify", false}, {"commitment", "processed"} } }) }
    };

    return sendJsonRpcRequest(payload);
}

std::string NetworkSolana::checkAccauntInfo(const std::string& address) {

    json payload = {
            {"jsonrpc", "2.0"},
            {"id", 1},
            {"method", "getAccountInfo"},
            {"params", {address}}
    };

    return sendJsonRpcRequest(payload);
}

std::string NetworkSolana::sendTransaction(const std::string& base64Transaction) {

    json payload = {
            {"jsonrpc", "2.0"},
            {"id", 1},
            {"method", "sendTransaction"},
            {"params", json::array({ base64Transaction }) }
    };

    return sendJsonRpcRequest(payload);
}


std::string NetworkSolana::simulateAndSendTransaction(const std::string& base64Transaction) {
    std::cout << "Simulating transaction..." << std::endl;
    std::string simResult = simulateTransaction(base64Transaction);
    std::cout << "Simulation result: " << simResult << std::endl;

    json simJson = json::parse(simResult, nullptr, false);
    if (simJson.contains("result") && simJson["result"].contains("err") && simJson["result"]["err"].is_null()) {
        std::cout << "Simulation successful. Sending transaction..." << std::endl;
        std::string sendResult = sendTransaction(base64Transaction);
        std::cout << "Send result: " << sendResult << std::endl;
        return sendResult;
    }

    std::cout << "Simulation failed. Transaction not sent." << std::endl;

}


std::string NetworkSolana::fetchRecentBlockhash() {
    json payload = {
            {"jsonrpc", "2.0"},
            {"id", 1},
            {"method", "getLatestBlockhash"},
            {"params", json::array()}
    };

    json result = json::parse(sendJsonRpcRequest(payload));
    return result["result"]["value"]["blockhash"].get<std::string>();
}

std::vector<unsigned char> NetworkSolana::createTransferInstruction(
        const std::vector<unsigned char>& from,
        const std::vector<unsigned char>& to,
        uint64_t lamports
) {
    std::vector<unsigned char> instruction;

    instruction.push_back(2);

    for (int i = 0; i < 8; ++i)
        instruction.push_back((lamports >> (8 * i)) & 0xFF);

    return instruction;
}

