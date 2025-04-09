#include "solana/solana_wallet.hpp"

#include "solana/solana_key_manager.hpp"
#include "solana/solana_network.hpp"
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

void SolanaWallet::generateKeyPair() {
    auto keys = SolanaKeyManager::generateKeyPair();
    privateKey = keys.privateKeyBase58;
    publicKey = keys.publicKeyBase58;
}

void SolanaWallet::showPublicAddress() const {
    std::cout << "Solana Public Address: " << publicKey << std::endl;
}

void SolanaWallet::checkBalance() const {
    auto resp = NetworkSolana::getBalance(publicKey);
    try {
        auto jsonResp = json::parse(resp);
        int lamports = jsonResp["result"]["value"].get<int>();
        std::cout << "Balance: " << lamports / 1000000000.0 << " SOL" << std::endl;
    } catch (...) {
        std::cout << "Failed to parse balance response." << std::endl;
    }
}
