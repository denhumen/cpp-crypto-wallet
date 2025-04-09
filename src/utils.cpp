//
// Created by denhumen on 4/9/25.
//

#include "utils.h"
#include <openssl/sha.h>
#include <openssl/ripemd.h>
#include <sstream>
#include <iomanip>
#include <vector>
#include <string>

namespace {

std::vector<unsigned char> hexToBytes(const std::string& hex) {
    std::vector<unsigned char> bytes;
    for (unsigned int i = 0; i < hex.length(); i += 2) {
        std::string byteString = hex.substr(i, 2);
        unsigned char byte = static_cast<unsigned char>(strtol(byteString.c_str(), nullptr, 16));
        bytes.push_back(byte);
    }
    return bytes;
}

std::vector<unsigned char> sha256(const std::vector<unsigned char>& data) {
    std::vector<unsigned char> hash(SHA256_DIGEST_LENGTH);
    SHA256(data.data(), data.size(), hash.data());
    return hash;
}

std::vector<unsigned char> ripemd160(const std::vector<unsigned char>& data) {
    std::vector<unsigned char> hash(RIPEMD160_DIGEST_LENGTH);
    RIPEMD160(data.data(), data.size(), hash.data());
    return hash;
}

const std::string BASE58_ALPHABET = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";

std::string base58Encode(const std::vector<unsigned char>& input) {
    std::vector<unsigned char> data = input;
    std::string result;
    unsigned long long int_val = 0;

    for (unsigned char byte : data) {
        int_val = (int_val << 8) | byte;
    }

    while (int_val > 0) {
        int rem = int_val % 58;
        int_val /= 58;
        result = BASE58_ALPHABET[rem] + result;
    }

    // Add '1' for each leading 0 byte
    for (unsigned char byte : data) {
        if (byte == 0x00)
            result = '1' + result;
        else
            break;
    }

    return result;
}

std::string toHex(const std::vector<unsigned char>& data) {
    std::stringstream ss;
    for (auto b : data) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)b;
    }
    return ss.str();
}

}

std::string Utils::base58CheckEncode(const std::string& dataHex) {
    auto data = hexToBytes(dataHex);
    auto firstHash = sha256(data);
    auto secondHash = sha256(firstHash);

    data.insert(data.end(), secondHash.begin(), secondHash.begin() + 4);
    return base58Encode(data);
}

std::string Utils::privateKeyToWIF(const std::string& hex, bool testnet) {
    std::string prefix = testnet ? "ef" : "80"; // 0xef for testnet, 0x80 for mainnet
    std::string full = prefix + hex + "01";     // Append 01 for compressed keys
    return base58CheckEncode(full);
}

std::string Utils::publicKeyToAddress(const std::string& pubKeyHex, bool testnet) {
    auto pubKeyBytes = hexToBytes(pubKeyHex);
    auto sha = sha256(pubKeyBytes);
    auto ripe = ripemd160(sha);

    std::vector<unsigned char> addressBytes;
    addressBytes.push_back(testnet ? 0x6f : 0x00); // Testnet prefix 0x6f
    addressBytes.insert(addressBytes.end(), ripe.begin(), ripe.end());

    auto firstHash = sha256(addressBytes);
    auto secondHash = sha256(firstHash);
    addressBytes.insert(addressBytes.end(), secondHash.begin(), secondHash.begin() + 4);

    return base58Encode(addressBytes);
}
