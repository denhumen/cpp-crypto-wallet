//
// Created by denhumen on 5/13/25.
//

#include "SolanaHelper.hpp"
#include "JsonRpcHelper.hpp"
#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;

const std::string SOLANA_RPC_URL = "https://api.devnet.solana.com";

std::string SolanaHelper::getBalance(const std::string& address) {
    json payload = {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "getBalance"},
        {"params", {address}}
    };

    std::string response = JsonRpcHelper::sendJsonRpcRequest(SOLANA_RPC_URL, payload);
    return response;
}

std::string SolanaHelper::requestAirdrop(const std::string& address, int64_t lamports) {
    json payload = {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "requestAirdrop"},
        {"params", {address, lamports}}
    };

    std::cout << "🚀 Requesting airdrop for: " << address << " with lamports: " << lamports << std::endl;
    std::string response = JsonRpcHelper::sendJsonRpcRequest(SOLANA_RPC_URL, payload);
    return response;
}
