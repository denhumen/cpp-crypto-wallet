//
// Created by denhumen on 5/14/25.
//

#include "SolanaWallet.hpp"
#include <iostream>
#include "JsonRpcHelper.hpp"
#include <nlohmann/json.hpp>
#include "encoding/Base58.hpp"
#include <sodium.h>

using json = nlohmann::json;
const std::string SOLANA_RPC_URL = "https://api.devnet.solana.com";

double SolanaWallet::getBalanceAsDouble(const std::string& address) const {
    json payload = {
        {"jsonrpc", "2.0"},
        {"id", 1},
        {"method", "getBalance"},
        {"params", {address}}
    };

    std::string response = JsonRpcHelper::sendJsonRpcRequest(SOLANA_RPC_URL, payload);
    try {
        json jsonResponse = json::parse(response);
        if (jsonResponse.contains("result") && jsonResponse["result"].contains("value")) {
            int64_t lamports = jsonResponse["result"]["value"];
            double solBalance = static_cast<double>(lamports) / 1e9; // Convert lamports to SOL
            return solBalance;
        }
    } catch (...) {
        return 0.0;
    }
    return 0.0;
}

std::string SolanaWallet::getBalanceAsString(const std::string& address) const {
    double balance = getBalanceAsDouble(address);

    std::ostringstream stream;
    stream << std::fixed << std::setprecision(5) << balance;
    std::string balanceStr = stream.str();

    balanceStr.erase(balanceStr.find_last_not_of('0') + 1);
    if (balanceStr.back() == '.') {
        balanceStr.pop_back();
    }

    balanceStr += " SOL";
    return balanceStr;
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

std::string SolanaWallet::signTransaction(
    TWHDWallet* wallet1,
    const std::string& toAddress,
    double amountSol
) const {
    const std::string mnemonic = "reflect ketchup move awful breeze regret require coyote language hood stamp amateur";
    TWHDWallet* wallet = TWHDWalletCreateWithMnemonic(TWStringCreateWithUTF8Bytes(mnemonic.c_str()), TWStringCreateWithUTF8Bytes(""));

    if (!wallet) throw std::runtime_error("Failed to create HD wallet");
    auto privKey = TWHDWalletGetKeyForCoin(wallet, TWCoinTypeSolana);
    auto seed    = TWPrivateKeyData(privKey);
    TWPrivateKeyDelete(privKey);
    TWHDWalletDelete(wallet);

    unsigned char sk[crypto_sign_SECRETKEYBYTES], pk[crypto_sign_PUBLICKEYBYTES];
    if (crypto_sign_seed_keypair(pk, sk, TWDataBytes(seed)) != 0) {
        throw std::runtime_error("crypto_sign_seed_keypair failed");
    }

    TWData* pkData = TWDataCreateWithBytes(pk, sizeof(pk));
    TWPublicKey* twPub = TWPublicKeyCreateWithData(pkData, TWPublicKeyTypeED25519);
    TWDataDelete(pkData);
    TWString* fromTw = TWCoinTypeDeriveAddressFromPublicKey(TWCoinTypeSolana, twPub);
    TWPublicKeyDelete(twPub);
    std::string fromAddress = TWStringUTF8Bytes(fromTw);
    TWStringDelete(fromTw);

    auto bhResp = json::parse(JsonRpcHelper::sendJsonRpcRequest(SOLANA_RPC_URL, {
            {"jsonrpc","2.0"}, {"id",1}, {"method","getLatestBlockhash"},
            {"params", {{{"commitment","finalized"}}}}
    }));
    std::string recentBlockhash = bhResp["result"]["value"]["blockhash"];
    int lamports = amountSol * 1000000000;
    std::cout << lamports << std::endl;
    // 5) Build TransactionMessage
    TransactionMessage msg;
    msg.num_required_signatures = 1;
    msg.num_readonly_signed     = 0;
    msg.num_readonly_unsigned   = 1;
    msg.account_keys = {
            Base58::decode(fromAddress),
            Base58::decode(toAddress),
            std::vector<uint8_t>(32, 0)
    };
    msg.recent_blockhash = Base58::decode(recentBlockhash);

    CompiledInstruction inst;
    inst.program_id_index = 2;
    inst.account_indices  = {0,1};
    inst.data.resize(4+8);
    inst.data[0] = 2; inst.data[1]=inst.data[2]=inst.data[3]=0;
    for (int i = 0; i < 8; ++i) {
        inst.data[4+i] = (lamports >> (8*i)) & 0xFF;
    }
    msg.instructions = { inst };

    auto serialized = serializeMessage(msg);
    std::vector<uint8_t> signature(crypto_sign_BYTES);
    if (crypto_sign_detached(signature.data(), nullptr,
                             serialized.data(), serialized.size(),
                             sk) != 0) {
        throw std::runtime_error("Failed to sign");
    }

    std::vector<uint8_t> fullTx;
    fullTx.push_back(1);
    fullTx.insert(fullTx.end(), signature.begin(), signature.end());
    fullTx.insert(fullTx.end(), serialized.begin(), serialized.end());

    size_t b64len = sodium_base64_ENCODED_LEN(fullTx.size(), sodium_base64_VARIANT_ORIGINAL);
    char* tmp = new char[b64len];
    sodium_bin2base64(
            tmp,
            b64len,
            fullTx.data(),
            fullTx.size(),
            sodium_base64_VARIANT_ORIGINAL
    );
    std::string txB64(tmp);
    delete[] tmp;

    auto sendResp = json::parse(JsonRpcHelper::sendJsonRpcRequest(SOLANA_RPC_URL, {
            {"jsonrpc","2.0"}, {"id",1}, {"method","sendTransaction"},
            {"params", json::array({ txB64, {{"encoding","base64"}} }) }
    }));
    if (sendResp.contains("error")) {
        return sendResp["error"].dump();
    }
    return sendResp["result"].get<std::string>();
}

