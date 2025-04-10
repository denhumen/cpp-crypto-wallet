//
// Created by denhumen on 4/9/25.
//

#ifndef SOLANANETWORK_HPP
#define SOLANANETWORK_HPP

#include <string>
#include <nlohmann/json.hpp>
#include "interfaces/INetwork.hpp"

class SolanaNetwork final : public INetwork {
    static std::string sendJsonRpcRequest(const nlohmann::json& payload);
public:
    [[nodiscard]] std::string getBalance(const std::string& address) const override;
    [[nodiscard]] std::string requestAirdrop(const std::string& address, int64_t lamports) const;

    [[nodiscard]] std::string simulateTransaction(const std::string& base64Transaction) const;
    [[nodiscard]] std::string checkAccauntInfo(const std::string& address) const;
    [[nodiscard]] std::string sendTransaction(const std::string& base64Transaction) const;
    [[nodiscard]] std::string simulateAndSendTransaction(const std::string& base64Transaction) const;
    [[nodiscard]] std::string fetchRecentBlockhash() const;
    [[nodiscard]] std::vector<unsigned char> createTransferInstruction(
            const std::vector<unsigned char>& from,
            const std::vector<unsigned char>& to,
            uint64_t lamports) const;
    ~SolanaNetwork() override;
};

#endif //SOLANANETWORK_HPP
