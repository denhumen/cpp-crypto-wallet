//
// Created by denhumen on 5/13/25.
//

#include "SolanaHelper.hpp"
#include "JsonRpcHelper.hpp"
#include <nlohmann/json.hpp>
#include <iostream>
#include <TrustWalletCore/TWHDWallet.h>
#include <TrustWalletCore/TWPrivateKey.h>
#include <TrustWalletCore/TWCoinType.h>
#include <TrustWalletCore/TWSolanaProto.h>
#include <TrustWalletCore/TWSolanaTransaction.h>
#include <HDWallet.h>
#include "encoding/Base58.hpp"
#include <sodium/crypto_sign.h>
#include <sodium/utils.h>

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

// Inline old serializeMessage:
static std::vector<uint8_t> serializeMessage(const TransactionMessage &msg) {
    std::vector<uint8_t> buf;
    buf.push_back(msg.num_required_signatures);
    buf.push_back(msg.num_readonly_signed);
    buf.push_back(msg.num_readonly_unsigned);
    buf.push_back(static_cast<uint8_t>(msg.account_keys.size()));
    for (auto &key: msg.account_keys) buf.insert(buf.end(), key.begin(), key.end());
    buf.insert(buf.end(), msg.recent_blockhash.begin(), msg.recent_blockhash.end());
    buf.push_back(static_cast<uint8_t>(msg.instructions.size()));
    for (auto &ins: msg.instructions) {
        buf.push_back(ins.program_id_index);
        buf.push_back(static_cast<uint8_t>(ins.account_indices.size()));
        buf.insert(buf.end(), ins.account_indices.begin(), ins.account_indices.end());
        buf.push_back(static_cast<uint8_t>(ins.data.size()));
        buf.insert(buf.end(), ins.data.begin(), ins.data.end());
    }
    return buf;
}

std::string SolanaHelper::transfer(
        const std::string& mnemonic,
        const std::string& toAddress,
        int64_t lamports
) {
    // 1) Derive 32-byte seed from mnemonic
    TWHDWallet* wallet = TWHDWalletCreateWithMnemonic(TWStringCreateWithUTF8Bytes(mnemonic.c_str()), TWStringCreateWithUTF8Bytes(""));

    if (!wallet) throw std::runtime_error("Failed to create HD wallet");
    auto privKey = TWHDWalletGetKeyForCoin(wallet, TWCoinTypeSolana);
    auto seed    = TWPrivateKeyData(privKey);
    TWPrivateKeyDelete(privKey);
    TWHDWalletDelete(wallet);

    // 2) Expand to full Ed25519 keypair via libsodium
    unsigned char sk[crypto_sign_SECRETKEYBYTES], pk[crypto_sign_PUBLICKEYBYTES];
    if (crypto_sign_seed_keypair(pk, sk, TWDataBytes(seed)) != 0) {
        throw std::runtime_error("crypto_sign_seed_keypair failed");
    }

    // 3) Get Base58 “from” address
    //    wrap pk in a TWPublicKey so TWSolanaAddressCreateWithPublicKey works:
    TWData* pkData = TWDataCreateWithBytes(pk, sizeof(pk));
    TWPublicKey* twPub = TWPublicKeyCreateWithData(pkData, TWPublicKeyTypeED25519);
    TWDataDelete(pkData);
    TWString* fromTw = TWCoinTypeDeriveAddressFromPublicKey(TWCoinTypeSolana, twPub);
    TWPublicKeyDelete(twPub);
    std::string fromAddress = TWStringUTF8Bytes(fromTw);
    TWStringDelete(fromTw);

    // 4) Fetch recent blockhash
    auto bhResp = json::parse(JsonRpcHelper::sendJsonRpcRequest(SOLANA_RPC_URL, {
            {"jsonrpc","2.0"}, {"id",1}, {"method","getLatestBlockhash"},
            {"params", {{{"commitment","finalized"}}}}
    }));
    std::string recentBlockhash = bhResp["result"]["value"]["blockhash"];

    // 5) Build TransactionMessage
    TransactionMessage msg;
    msg.num_required_signatures = 1;
    msg.num_readonly_signed     = 0;
    msg.num_readonly_unsigned   = 1;
    msg.account_keys = {
            Base58::decode(fromAddress),
            Base58::decode(toAddress),
            std::vector<uint8_t>(32, 0)  // system program
    };
    msg.recent_blockhash = Base58::decode(recentBlockhash);

    CompiledInstruction inst;
    inst.program_id_index = 2;
    inst.account_indices  = {0,1};
    inst.data.resize(4+8);
    // 4-byte LE instruction = 2
    inst.data[0] = 2; inst.data[1]=inst.data[2]=inst.data[3]=0;
    // 8-byte LE lamports
    for (int i = 0; i < 8; ++i) {
        inst.data[4+i] = (lamports >> (8*i)) & 0xFF;
    }
    msg.instructions = { inst };

    // 6) Serialize & sign
    auto serialized = serializeMessage(msg);
    std::vector<uint8_t> signature(crypto_sign_BYTES);
    if (crypto_sign_detached(signature.data(), nullptr,
                             serialized.data(), serialized.size(),
                             sk) != 0) {
        throw std::runtime_error("Failed to sign");
    }

    // 7) Build fullTx = [1 || signature || serialized]
    std::vector<uint8_t> fullTx;
    fullTx.push_back(1);
    fullTx.insert(fullTx.end(), signature.begin(), signature.end());
    fullTx.insert(fullTx.end(), serialized.begin(), serialized.end());

    // 8) Base64 encode
    size_t b64len = sodium_base64_ENCODED_LEN(fullTx.size(), sodium_base64_VARIANT_ORIGINAL);
    char* tmp = new char[b64len];  // includes room for NUL
    sodium_bin2base64(
            tmp,
            b64len,
            fullTx.data(),
            fullTx.size(),
            sodium_base64_VARIANT_ORIGINAL
    );
    // Now tmp is a \0-terminated C-string:
    std::string txB64(tmp);
    delete[] tmp;

    // 9) Send via JSON-RPC
    auto sendResp = json::parse(JsonRpcHelper::sendJsonRpcRequest(SOLANA_RPC_URL, {
            {"jsonrpc","2.0"}, {"id",1}, {"method","sendTransaction"},
            {"params", json::array({ txB64, {{"encoding","base64"}} }) }
    }));
    if (sendResp.contains("error")) {
        throw std::runtime_error(sendResp["error"].dump());
    }
    return sendResp["result"].get<std::string>();
}
