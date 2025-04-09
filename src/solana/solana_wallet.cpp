#include "solana/solana_wallet.hpp"

#include "solana/solana_key_manager.hpp"
#include "solana/solana_network.hpp"
#include <iostream>
#include <nlohmann/json.hpp>
#include <thread>

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
//    try {
        auto jsonResp = json::parse(resp);

        if (jsonResp.contains("result") && jsonResp["result"].contains("value") && !jsonResp["result"]["value"].is_null()) {
            int lamports = jsonResp["result"]["value"].get<int>();
            std::cout << "Balance: " << lamports / 1000000000.0 << " SOL" << std::endl;
        } else {
            std::cout << "No balance found (account may not exist on chain)." << std::endl;
        }
//    } catch (const std::exception& e) {
//        std::cout << "Failed to parse balance response: " << e.what() << std::endl;
//    }
}

void SolanaWallet::airdropAndCheckBalance() {
    std::cout << "Requesting airdrop to " << publicKey << "..." << std::endl;
    NetworkSolana::requestAirdrop(publicKey, 1000000000);
    // 1 SOL
    for(int i = 0; i < 5; i++){
        std::this_thread::sleep_for(std::chrono::seconds(3)); // wait for airdrop

        checkBalance();
    }
}