#include "solana/solana_key_manager.hpp"
#include <sodium.h>
#include <stdexcept>
#include <iostream>
#include "solana/solana_utils.hpp"

SolanaKeyPair SolanaKeyManager::generateKeyPair() {
    SolanaKeyPair kp;

    unsigned char publicKey[crypto_sign_PUBLICKEYBYTES];
    unsigned char secretKey[crypto_sign_SECRETKEYBYTES];

    crypto_sign_keypair(publicKey, secretKey);

    kp.privateKeyBase64 = std::string(reinterpret_cast<char*>(secretKey), crypto_sign_SECRETKEYBYTES);

    kp.publicKeyBase58 = UtilsSolana::base58Encode(publicKey, crypto_sign_PUBLICKEYBYTES);
//    kp.publicKeyBase58 = std::string(reinterpret_cast<char*>(publicKey), crypto_sign_PUBLICKEYBYTES);

    std::cout << "Base64 privateKey: " << kp.privateKeyBase64 << std::endl;


    return kp;
}


std::string SolanaKeyManager::base64Encode(const std::vector<unsigned char>& data) {
    std::string encoded;
    encoded.resize(sodium_base64_ENCODED_LEN(data.size(), sodium_base64_VARIANT_ORIGINAL));
    sodium_bin2base64(&encoded[0], encoded.size(), data.data(), data.size(), sodium_base64_VARIANT_ORIGINAL);
    return encoded;
}

std::vector<unsigned char> SolanaKeyManager::base64Decode(const std::string& encoded) {
    std::vector<unsigned char> decoded(128);
    size_t outLen = 0;
    int res = sodium_base642bin(decoded.data(), decoded.size(), encoded.c_str(), encoded.size(), nullptr, &outLen, nullptr, sodium_base64_VARIANT_ORIGINAL);
//    if (res != 0) {
//        throw std::runtime_error("Invalid base64 input");
//    }
    std::cout << res << std::endl;
    decoded.resize(outLen);
    return decoded;
}
