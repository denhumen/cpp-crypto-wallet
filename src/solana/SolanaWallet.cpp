//
// Created by denhumen on 4/9/25.
//

#include "solana/SolanaWallet.hpp"
#include "solana/SolanaNetwork.hpp"
#include "solana/SolanaKeyManager.hpp"
#include <iostream>
#include <nlohmann/json.hpp>
#include <thread>
#include <sodium/crypto_sign.h>
#include <sodium/utils.h>

#include "Base58.hpp"

using json = nlohmann::json;

SolanaWallet::SolanaWallet()
    : keyManager(std::make_unique<SolanaKeyManager>()), network(std::make_unique<SolanaNetwork>())
{}


void SolanaWallet::generateKeyPair() {
    auto keys = keyManager->generateKeyPair();
    privateKey = keys.privateKey;
    publicKey = keys.publicKey;
}

void SolanaWallet::showPublicAddress() const {
    std::cout << "Solana Public Address: " << publicKey << std::endl;
}

void SolanaWallet::showPrivateAddress() const {
    std::cout << "Solana Private Address: " << privateKey << std::endl;
}

void SolanaWallet::checkBalance() const {
    auto resp = network->getBalance(publicKey);
    auto jsonResp = json::parse(resp);

    if (jsonResp.contains("result") && jsonResp["result"].contains("value") && !jsonResp["result"]["value"].is_null()) {
        long long lamports = jsonResp["result"]["value"].get<long long>();
        std::cout << "Balance: " << lamports / 1000000000.0 << " SOL" << std::endl;
    } else {
        std::cout << "No balance found (account may not exist on chain)." << std::endl;
    }
}

void SolanaWallet::airdropAndCheckBalance(long amount) const {
    std::cout << "Requesting airdrop to " << publicKey << "..." << std::endl;
    const auto response = network->requestAirdrop(publicKey, amount * 1000000000);
    std::cout << "Response: " << response << std::endl;
    for(int i = 0; i < 5; i++){
        std::this_thread::sleep_for(std::chrono::seconds(3));

        checkBalance();
    }
}

void SolanaWallet::setPrivateKey(const std::string& newPrivate) {
    std::vector<uint8_t> decoded = Base58::decode(newPrivate);
    privateKey = std::string(decoded.begin(), decoded.end());
}

void SolanaWallet::setPublicKey(const std::string& newPublic) {
    publicKey = newPublic;
}

void SolanaWallet::sendSomeToOtherWallet(std::string recipientPub58, long amount) {

    std::string recentBlockhash = network->fetchRecentBlockhash();
    std::cout << "Blockhash: " << recentBlockhash << std::endl;

    std::string txid = createTransactionId(privateKey, publicKey,
                                         recipientPub58, recentBlockhash, amount * 1000000000ULL);


    std::string res = network->simulateAndSendTransaction(txid);
}

std::string SolanaWallet::checkAccountInfo() {
    return network->checkAccauntInfo(publicKey);
}

std::vector<uint8_t> SolanaWallet::serializeMessage(const TransactionMessage &msg) {
    std::vector<uint8_t> buffer;
    buffer.push_back(msg.num_required_signatures);
    buffer.push_back(msg.num_readonly_signed);
    buffer.push_back(msg.num_readonly_unsigned);

    if (msg.account_keys.size() > 0x7F)
        throw std::runtime_error("Too many account keys for this simple serializer.");
    buffer.push_back(static_cast<uint8_t>(msg.account_keys.size()));
    for (const auto &key : msg.account_keys) {
        if (key.size() != 32)
            throw std::runtime_error("Each account key must be 32 bytes.");
        buffer.insert(buffer.end(), key.begin(), key.end());
    }

    if (msg.recent_blockhash.size() != 32)
        throw std::runtime_error("Recent blockhash must be 32 bytes.");
    buffer.insert(buffer.end(), msg.recent_blockhash.begin(), msg.recent_blockhash.end());

    if (msg.instructions.size() > 0x7F)
        throw std::runtime_error("Too many instructions for this serializer.");
    buffer.push_back(static_cast<uint8_t>(msg.instructions.size()));
    for (const auto &ins : msg.instructions) {
        buffer.push_back(ins.program_id_index);

        if (ins.account_indices.size() > 0x7F)
            throw std::runtime_error("Too many account indices.");
        buffer.push_back(static_cast<uint8_t>(ins.account_indices.size()));
        buffer.insert(buffer.end(), ins.account_indices.begin(), ins.account_indices.end());

        if (ins.data.size() > 0x7F)
            throw std::runtime_error("Instruction data too long.");
        buffer.push_back(static_cast<uint8_t>(ins.data.size()));
        buffer.insert(buffer.end(), ins.data.begin(), ins.data.end());
    }

    return buffer;
}

std::vector<uint8_t> SolanaWallet::signMessage(const std::vector<uint8_t>& message, const std::string &secret_key_str) {
    if (secret_key_str.size() != crypto_sign_SECRETKEYBYTES)
        throw std::runtime_error("Invalid secret key length; expected 64 bytes.");
    std::vector<uint8_t> signature(crypto_sign_BYTES);
    if (crypto_sign_detached(signature.data(), nullptr, message.data(), message.size(),
                             reinterpret_cast<const unsigned char*>(secret_key_str.data())) != 0) {
        throw std::runtime_error("Failed to sign the message.");
                             }
    return signature;
}

std::string SolanaWallet::createTransactionId(const std::string &senderSecretKey,
                              const std::string &senderPublicKey,
                              const std::string &receiverPublicKey,
                              const std::string &recentBlockhash,
                              uint64_t lamports) {
    TransactionMessage msg;
    msg.num_required_signatures = 1;
    msg.num_readonly_signed = 0;
    msg.num_readonly_unsigned = 1;

    std::vector<uint8_t> senderVec = Base58::decode(senderPublicKey);
    std::vector<uint8_t> receiverVec = Base58::decode(receiverPublicKey);
    if (senderVec.size() != 32 || receiverVec.size() != 32) {
        throw std::runtime_error("Public keys must be 32 bytes each.");
    }
    msg.account_keys.push_back(senderVec);
    msg.account_keys.push_back(receiverVec);

    std::vector<uint8_t> systemProgramId(32, 0);
    msg.account_keys.push_back(systemProgramId);

    std::vector<uint8_t> blockhashVec = Base58::decode(recentBlockhash);
    if (blockhashVec.size() != 32)
        throw std::runtime_error("Recent blockhash must be 32 bytes.");
    msg.recent_blockhash = blockhashVec;

    CompiledInstruction inst;
    inst.program_id_index = 2;

    inst.account_indices.push_back(0);
    inst.account_indices.push_back(1);

    inst.data.clear();

    for (int i = 0; i < 4; i++) {
        constexpr uint32_t transferInstruction = 2;
        inst.data.push_back(static_cast<uint8_t>((transferInstruction >> (8 * i)) & 0xFF));
    }

    // Push 8 bytes for lamports (also little-endian)
    for (int i = 0; i < 8; i++) {
        inst.data.push_back(static_cast<uint8_t>((lamports >> (8 * i)) & 0xFF));
    }

    msg.instructions.push_back(inst);

    std::vector<uint8_t> serialized = serializeMessage(msg);

    std::vector<uint8_t> signature = signMessage(serialized, senderSecretKey);

    std::vector<uint8_t> fullTx;
    fullTx.push_back(1); // One signature.
    fullTx.insert(fullTx.end(), signature.begin(), signature.end());
    fullTx.insert(fullTx.end(), serialized.begin(), serialized.end());

    char* txBase64_cstr = new char[sodium_base64_ENCODED_LEN(fullTx.size(), sodium_base64_VARIANT_ORIGINAL)];
    sodium_bin2base64(txBase64_cstr, sodium_base64_ENCODED_LEN(fullTx.size(), sodium_base64_VARIANT_ORIGINAL),
                      fullTx.data(), fullTx.size(), sodium_base64_VARIANT_ORIGINAL);
    std::string txBase64(txBase64_cstr);
    delete[] txBase64_cstr;
    return txBase64;
}
