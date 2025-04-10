//
// Created by denhumen on 4/9/25.
//

#ifndef SOLANAWALLET_HPP
#define SOLANAWALLET_HPP

#pragma once
#include "interfaces/IWallet.hpp"
#include "SolanaKeyManager.hpp"
#include "SolanaNetwork.hpp"
#include <memory>

struct CompiledInstruction {
    uint8_t program_id_index;
    std::vector<uint8_t> account_indices;
    std::vector<uint8_t> data;
};

struct TransactionMessage {
    uint8_t num_required_signatures;
    uint8_t num_readonly_signed;
    uint8_t num_readonly_unsigned;
    std::vector<std::vector<uint8_t>> account_keys;
    std::vector<uint8_t> recent_blockhash;
    std::vector<CompiledInstruction> instructions;
};

class SolanaWallet final : public IWallet {
public:
    SolanaWallet();
    void generateKeyPair() override;
    void showPublicAddress() const override;
    void showPrivateAddress() const override;
    void checkBalance() const override;

    void airdropAndCheckBalance(long amount) const override;
    ~SolanaWallet() override = default;

    void setPrivateKey(const std::string& newPrivate);
    void setPublicKey(const std::string& newPublic);

    void sendSomeToOtherWallet(std::string recipientPub58, long amount);
    std::string checkAccountInfo();

private:
    std::string privateKey;
    std::string publicKey;

    std::unique_ptr<SolanaKeyManager> keyManager;
    std::unique_ptr<SolanaNetwork> network;

    std::vector<uint8_t> serializeMessage(const TransactionMessage &msg);
    std::vector<uint8_t> signMessage(const std::vector<uint8_t> &message, const std::string &secret_key_str);
    std::string createTransactionId(const std::string &senderSecretKey, const std::string &senderPublicKey,
                                    const std::string &receiverPublicKey, const std::string &recentBlockhash,
                                    uint64_t lamports);
};

#endif //SOLANAWALLET_HPP
