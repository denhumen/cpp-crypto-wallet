//
// Created by denhumen on 5/14/25.
//

#include "EthereumWallet.hpp"
#include "JsonRpcHelper.hpp"

#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <array>
#include <cstdint>
#include <cctype>
#include <stdexcept>

#include <nlohmann/json.hpp>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <secp256k1.h>
#include <secp256k1_recovery.h>

#include <TrustWalletCore/TWHDWallet.h>
#include <TrustWalletCore/TWPrivateKey.h>
#include <TrustWalletCore/TWData.h>
#include <TrustWalletCore/TWString.h>
#include <TrustWalletCore/TWAnyAddress.h>
#include <TrustWalletCore/TWCoinType.h>

using json = nlohmann::json;

static const std::string ETHERSCAN_URL = "https://api-sepolia.etherscan.io/api";
static const std::string API_TOKEN     = "9CFWDYF2WJNEDGAHG31UU5PK99RGJ1UINV";
static constexpr uint64_t CHAIN_ID     = 11155111;  // Sepolia

static std::string toHex(const std::vector<uint8_t>& data) {
    static constexpr char hex[] = "0123456789abcdef";
    std::string out; out.reserve(data.size()*2 + 2);
    out = "0x";
    for (auto b: data) {
        out.push_back(hex[b >> 4]);
        out.push_back(hex[b & 0xF]);
    }
    return out;
}

static std::array<uint8_t,32> keccak256(const uint8_t* data, size_t len) {
    std::array<uint8_t,32> hash;
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(ctx, EVP_sha3_256(), nullptr);
    EVP_DigestUpdate(ctx, data, len);
    unsigned int outLen;
    EVP_DigestFinal_ex(ctx, hash.data(), &outLen);
    EVP_MD_CTX_free(ctx);
    return hash;
}

static void appendRlpEncodedUint(std::vector<uint8_t>& out, uint64_t value) {
    if (value == 0) {
        out.push_back(0x80);
        return;
    }
    std::vector<uint8_t> buf;
    while (value) {
        buf.push_back(static_cast<uint8_t>(value & 0xFF));
        value >>= 8;
    }
    std::reverse(buf.begin(), buf.end());
    if (buf.size() == 1 && buf[0] < 0x80) {
        out.push_back(buf[0]);
    } else {
        out.push_back(static_cast<uint8_t>(0x80 + buf.size()));
        out.insert(out.end(), buf.begin(), buf.end());
    }
}

static void appendRlpEncodedBytes(std::vector<uint8_t>& out, const std::vector<uint8_t>& buf) {
    if (buf.size() == 1 && buf[0] < 0x80) {
        out.push_back(buf[0]);
    } else {
        out.push_back(static_cast<uint8_t>(0x80 + buf.size()));
        out.insert(out.end(), buf.begin(), buf.end());
    }
}

static std::vector<uint8_t> trimLeadingZeros(std::vector<uint8_t> v) {
    size_t i = 0;
    while (i + 1 < v.size() && v[i] == 0) i++;
    return { v.begin() + i, v.end() };
}

static std::vector<uint8_t> hexStringToBytes(const std::string& hex) {
    size_t start = 0;
    if (hex.size() >= 2 && hex[0]=='0' && std::tolower(hex[1])=='x') start = 2;
    size_t len = hex.size() - start;
    if (len % 2) throw std::invalid_argument("hex string must have even length");
    std::vector<uint8_t> bytes; bytes.reserve(len/2);
    auto hexChar = [&](char c)->int {
        if (c>='0' && c<='9') return c - '0';
        c = std::tolower(c);
        if (c>='a' && c<='f') return 10 + (c - 'a');
        throw std::invalid_argument("invalid hex character");
    };
    for (size_t i = start; i < hex.size(); i += 2) {
        int hi = hexChar(hex[i]);
        int lo = hexChar(hex[i+1]);
        bytes.push_back(static_cast<uint8_t>((hi<<4)|lo));
    }
    return bytes;
}


double EthereumWallet::getBalanceAsDouble(const std::string& address) const {
    auto url = ETHERSCAN_URL
      + "?module=account"
      + "&action=balance"
      + "&address=" + address
      + "&tag=latest"
      + "&apikey=" + API_TOKEN;
    auto resp = JsonRpcHelper::sendApiRequest(url);
    std::cout << "[DEBUG] etherscan balance → " << resp << "\n";

    auto j = json::parse(resp);
    if (j.value("status","0") != "1") {
        throw std::runtime_error(
            "etherscan balance error: " +
            j.value("message","unknown")
        );
    }
    uint64_t wei = std::stoull(j["result"].get<std::string>());
    return double(wei) / 1e18;
}

std::string EthereumWallet::getBalanceAsString(const std::string& address) const {
    double b = getBalanceAsDouble(address);
    std::ostringstream os;
    os << std::fixed << std::setprecision(5) << b << " ETH";
    return os.str();
}

std::string EthereumWallet::signTransaction(
    TWHDWallet* wallet,
    const std::string& toAddress,
    double amountEth
) const {

    std::cout << TWStringUTF8Bytes(TWHDWalletGetAddressForCoin(wallet, TWCoinTypeEthereum)) << std::endl;
    auto pkObj  = TWHDWalletGetKeyForCoin(wallet, TWCoinTypeEthereum);
    TWData* pd  = TWPrivateKeyData(pkObj);
    auto privKey = static_cast<const uint8_t*>(TWDataBytes(pd));
    size_t keyLen= TWDataSize(pd);
    TWDataDelete(pd);
    TWPrivateKeyDelete(pkObj);

    TWString* fromTw = TWHDWalletGetAddressForCoin(wallet, TWCoinTypeEthereum);
    std::string fromAddress = TWStringUTF8Bytes(fromTw);
    TWStringDelete(fromTw);
    std::cout << "[DEBUG] fromAddress: " << fromAddress << "\n";

    {
        auto url = ETHERSCAN_URL
          + "?module=proxy"
          + "&action=eth_getTransactionCount"
          + "&address=" + fromAddress
          + "&tag=latest"
          + "&apikey=" + API_TOKEN;
        auto r = JsonRpcHelper::sendApiRequest(url);
        std::cout << "[DEBUG] eth_getTransactionCount → " << r << "\n";
        auto j = json::parse(r);
        if (j.contains("error")) {
            throw std::runtime_error("Nonce error: " + j["error"]["message"].get<std::string>());
        }
        uint64_t nonce = std::stoull(j.value("result","0x0").substr(2), nullptr, 16);

        auto url2 = ETHERSCAN_URL
          + "?module=proxy"
          + "&action=eth_gasPrice"
          + "&apikey=" + API_TOKEN;
        auto r2 = JsonRpcHelper::sendApiRequest(url2);
        std::cout << "[DEBUG] eth_gasPrice → " << r2 << "\n";
        auto j2 = json::parse(r2);
        if (j2.contains("error")) {
            throw std::runtime_error("GasPrice error: " + j2["error"]["message"].get<std::string>());
        }
        uint64_t gasPrice = std::stoull(j2.value("result","0x0").substr(2), nullptr, 16) / 2;  // half

        uint64_t gasLimit = 21000;
        uint64_t valueWei = static_cast<uint64_t>(amountEth * 1e18);

        auto toBytes = hexStringToBytes(toAddress);
        std::vector<uint8_t> rlp;
        appendRlpEncodedUint(rlp, nonce);
        appendRlpEncodedUint(rlp, gasPrice);
        appendRlpEncodedUint(rlp, gasLimit);
        appendRlpEncodedBytes(rlp, toBytes);
        appendRlpEncodedUint(rlp, valueWei);
        rlp.push_back(0x80);
        appendRlpEncodedUint(rlp, CHAIN_ID);
        appendRlpEncodedUint(rlp, 0);
        appendRlpEncodedUint(rlp, 0);

        std::vector<uint8_t> enc;
        if (rlp.size()<56) {
            enc.push_back(static_cast<uint8_t>(0xc0 + rlp.size()));
        } else {
            std::vector<uint8_t> lb; appendRlpEncodedUint(lb, rlp.size());
            enc.push_back(static_cast<uint8_t>(0xf7 + lb.size()));
            enc.insert(enc.end(), lb.begin(), lb.end());
        }
        enc.insert(enc.end(), rlp.begin(), rlp.end());

        auto hash = keccak256(enc.data(), enc.size());

        secp256k1_context* ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN);
        secp256k1_ecdsa_recoverable_signature sig;
        secp256k1_ecdsa_sign_recoverable(
            ctx, &sig,
            hash.data(), privKey,
            nullptr, nullptr
        );
        uint8_t sig64[64];
        int recid;
        secp256k1_ecdsa_recoverable_signature_serialize_compact(
            ctx, sig64, &recid, &sig
        );
        secp256k1_context_destroy(ctx);

        std::vector<uint8_t> r_sig(sig64,         sig64 + 32);
        std::vector<uint8_t> s_sig(sig64 + 32,    sig64 + 64);
        uint64_t v = recid + 35 + 2 * CHAIN_ID;  // EIP-155

        std::vector<uint8_t> rlp2;
        appendRlpEncodedUint(rlp2, nonce);
        appendRlpEncodedUint(rlp2, gasPrice);
        appendRlpEncodedUint(rlp2, gasLimit);
        appendRlpEncodedBytes(rlp2, toBytes);
        appendRlpEncodedUint(rlp2, valueWei);
        rlp2.push_back(0x80);               // empty data
        appendRlpEncodedUint(rlp2, v);
        appendRlpEncodedBytes(rlp2, trimLeadingZeros(r_sig));
        appendRlpEncodedBytes(rlp2, trimLeadingZeros(s_sig));

        std::vector<uint8_t> finalTx;
        if (rlp2.size()<56) {
            finalTx.push_back(static_cast<uint8_t>(0xc0 + rlp2.size()));
        } else {
            std::vector<uint8_t> lb; appendRlpEncodedUint(lb, rlp2.size());
            finalTx.push_back(static_cast<uint8_t>(0xf7 + lb.size()));
            finalTx.insert(finalTx.end(), lb.begin(), lb.end());
        }
        finalTx.insert(finalTx.end(), rlp2.begin(), rlp2.end());

        auto rawHex = toHex(finalTx);
        std::string proxyUrl =
            ETHERSCAN_URL
          + "?module=proxy"
          + "&action=eth_sendRawTransaction"
          + "&hex="   + rawHex
          + "&apikey="+ API_TOKEN;
        std::cout << "[DEBUG] etherscan sendRaw URL → " << proxyUrl << "\n";
        auto proxyRes = JsonRpcHelper::sendApiRequest(proxyUrl);
        std::cout << "[DEBUG] etherscan sendRaw → " << proxyRes << "\n";
        try {
            auto proxyJ = json::parse(proxyRes);
            if (proxyJ.value("status","0") != "1") {
                throw std::runtime_error(
                    "Etherscan proxy error: " + proxyJ.value("message","unknown")
                );
            }
            return proxyJ["result"].get<std::string>();
        }
        catch (const std::exception& e) {
            std::cerr << "[Sending error] response → " << proxyRes << "\n";
            return "";
        }
    }
}
