#ifndef CPP_CRYPTO_WALLET_SOLANA_NETWORK_HPP
#define CPP_CRYPTO_WALLET_SOLANA_NETWORK_HPP

#include <string>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

class NetworkSolana {
    std::string sendJsonRpcRequest(const json& payload);
public:
    std::string getBalance(const std::string& base58Address);
    std::string requestAirdrop(const std::string& address, int64_t lamports);
    std::string simulateTransaction(const std::string& base64Transaction);
    std::string checkAccauntInfo(const std::string& address);
    std::string sendTransaction(const std::string& base64Transaction);
    std::string simulateAndSendTransaction(const std::string& base64Transaction);
    std::string fetchRecentBlockhash();
    std::vector<unsigned char> createTransferInstruction(
            const std::vector<unsigned char>& from,
            const std::vector<unsigned char>& to,
            uint64_t lamports);
};

#endif //CPP_CRYPTO_WALLET_SOLANA_NETWORK_HPP
