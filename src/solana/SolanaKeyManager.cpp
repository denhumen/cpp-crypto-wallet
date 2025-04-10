//
// Created by denhumen on 4/9/25.
//

#include "solana/SolanaKeyManager.hpp"

#include <iostream>
#include <memory>

#include "encoding/Base58.hpp"
#include <sodium.h>

KeyPair SolanaKeyManager::generateKeyPair() {
    KeyPair kp{};

    const auto publicKey = std::make_unique<unsigned char[]>(crypto_sign_PUBLICKEYBYTES);
    const auto secretKey = std::make_unique<unsigned char[]>(crypto_sign_SECRETKEYBYTES);

    crypto_sign_keypair(publicKey.get(), secretKey.get());

    kp.privateKey = std::string(reinterpret_cast<char*>(secretKey.get()), crypto_sign_SECRETKEYBYTES);

    const std::vector<unsigned char> data(publicKey.get(), publicKey.get() + crypto_sign_PUBLICKEYBYTES);
    kp.publicKey = Base58::encode(data);

    return kp;
}

SolanaKeyManager::~SolanaKeyManager() = default;