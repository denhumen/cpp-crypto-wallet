#include "solana/solana_wallet.hpp"

#include "solana/solana_key_manager.hpp"
#include "solana/solana_network.hpp"
#include "base58.hpp"
#include <iostream>
#include <nlohmann/json.hpp>
#include <thread>
#include <sodium.h>

using json = nlohmann::json;


struct CompiledInstruction {
    uint8_t program_id_index;
    std::vector<uint8_t> account_indices;
    std::vector<uint8_t> data;
};

struct TransactionMessage {
    uint8_t num_required_signatures;    // usually 1 for a basic transfer
    uint8_t num_readonly_signed;
    uint8_t num_readonly_unsigned;
    std::vector<std::vector<uint8_t>> account_keys; // each 32 bytes
    std::vector<uint8_t> recent_blockhash;          // 32 bytes
    std::vector<CompiledInstruction> instructions;
};


std::vector<uint8_t> toVector(const std::string &s) {
    return std::vector<uint8_t>(s.begin(), s.end());
}
std::string base58_encode(const std::vector<uint8_t>& input) {
    const char* alphabet = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";
    std::vector<uint8_t> digits = {0};
    for (size_t i = 0; i < input.size(); i++) {
        int carry = input[i];
        for (size_t j = 0; j < digits.size(); j++) {
            int x = digits[j] * 256 + carry;
            digits[j] = x % 58;
            carry = x / 58;
        }
        while (carry) {
            digits.push_back(carry % 58);
            carry /= 58;
        }
    }
    // Account for leading zeros
    for (size_t i = 0; i < input.size() && input[i] == 0; i++) {
        digits.push_back(0);
    }
    std::string result;
    // Since digits are in little-endian order, reverse them.
    for (auto it = digits.rbegin(); it != digits.rend(); ++it) {
        result.push_back(alphabet[*it]);
    }
    return result;
}

std::vector<uint8_t> serializeMessage(const TransactionMessage &msg) {
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
        // Program id index
        buffer.push_back(ins.program_id_index);

        // Account indices (serialize length then data)
        if (ins.account_indices.size() > 0x7F)
            throw std::runtime_error("Too many account indices.");
        buffer.push_back(static_cast<uint8_t>(ins.account_indices.size()));
        buffer.insert(buffer.end(), ins.account_indices.begin(), ins.account_indices.end());

        // Instruction data (serialize length then data)
        if (ins.data.size() > 0x7F)
            throw std::runtime_error("Instruction data too long.");
        buffer.push_back(static_cast<uint8_t>(ins.data.size()));
        buffer.insert(buffer.end(), ins.data.begin(), ins.data.end());
    }

    return buffer;
}

std::vector<uint8_t> signMessage(const std::vector<uint8_t>& message, const std::string &secret_key_str) {
    if (secret_key_str.size() != crypto_sign_SECRETKEYBYTES)
        throw std::runtime_error("Invalid secret key length; expected 64 bytes.");
    std::vector<uint8_t> signature(crypto_sign_BYTES);
    if (crypto_sign_detached(signature.data(), nullptr, message.data(), message.size(),
                             reinterpret_cast<const unsigned char*>(secret_key_str.data())) != 0) {
        throw std::runtime_error("Failed to sign the message.");
    }
    return signature;
}

std::string createTransaction(const std::string &senderSecretKey,  // raw 64-byte secret key
                              const std::string &senderPublicKey,  // raw 32-byte public key
                              const std::string &receiverPublicKey, // raw 32-byte public key
                              const std::string &recentBlockhash,  // raw 32-byte blockhash
                              uint64_t lamports) {
    TransactionMessage msg;
    msg.num_required_signatures = 1;
    msg.num_readonly_signed = 0;
    msg.num_readonly_unsigned = 1;

    std::vector<uint8_t> senderPub = Base58::decode(senderPublicKey);
    std::vector<uint8_t> recipientPub = Base58::decode(receiverPublicKey);

    std::vector<uint8_t> senderVec = senderPub;
    std::vector<uint8_t> receiverVec = recipientPub;
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

    inst.data.push_back(2);
    for (int i = 0; i < 8; i++) {
        inst.data.push_back(static_cast<uint8_t>((lamports >> (8 * i)) & 0xFF));
    }
    msg.instructions.push_back(inst);

    std::vector<uint8_t> serialized = serializeMessage(msg);

    std::vector<uint8_t> signature = signMessage(serialized, senderSecretKey);

// Build the full transaction:
    //  [signature count (1 byte)] [signature (64 bytes)] [serialized message]
    std::vector<uint8_t> fullTx;
    fullTx.push_back(1); // One signature.
    fullTx.insert(fullTx.end(), signature.begin(), signature.end());
    fullTx.insert(fullTx.end(), serialized.begin(), serialized.end());

    // Encode the full transaction in Base64 (which is what the RPC expects).
//    std::string txBase64;
//    txBase64.resize(sodium_base64_ENCODED_LEN(fullTx.size(), sodium_base64_VARIANT_ORIGINAL));
//    sodium_bin2base64(&txBase64[0], txBase64.size(), fullTx.data(), fullTx.size(), sodium_base64_VARIANT_ORIGINAL);

    return Base58::encode(fullTx);
}

void SolanaWallet::generateKeyPair() {
    auto keys = SolanaKeyManager::generateKeyPair();
    privateKey = keys.privateKeyBase64;
    publicKey = keys.publicKeyBase58;
}

void SolanaWallet::showPublicAddress() const {
    std::cout << "Solana Public Address: " << publicKey << std::endl;
}

void SolanaWallet::checkBalance() {
    auto resp = solanaNet.getBalance(publicKey);
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

void SolanaWallet::airdropAndCheckBalance(long amount) {
    std::cout << "Requesting airdrop to " << publicKey << "..." << std::endl;
    std::cout << "Response: " << solanaNet.requestAirdrop(publicKey, amount * 1000000000)   << "..." << std::endl;

    for(int i = 0; i < 5; i++){
        std::this_thread::sleep_for(std::chrono::seconds(3));

        checkBalance();
    }
}

void SolanaWallet::sendSomeToOtherWallet(std::string recipientPub58, long amount) {

    std::string recentBlockhash = solanaNet.fetchRecentBlockhash();
    std::cout << "Blockhash: " << recentBlockhash << std::endl;

    std::string txid = createTransaction(privateKey, publicKey,
                                         recipientPub58, recentBlockhash, amount * 1000000000ULL);


    std::string res = solanaNet.simulateAndSendTransaction(txid);
//    std::cout << "Result of transaction: " << res << std::endl;
}

void SolanaWallet::loopCheckBalance() {

    while(true){
        std::this_thread::sleep_for(std::chrono::seconds(3));
        std::cout << checkAccountInfo() << std::endl;
        checkBalance();
    }
}

std::string SolanaWallet::checkAccountInfo() {
    return solanaNet.checkAccauntInfo("DN9rcSUEhNEznpocHP5V1APs6YNxR568QtTgSYTnDBwW");
}
