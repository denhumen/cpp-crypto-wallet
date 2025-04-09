//
// Created by denhumen on 4/9/25.
//

#include "wallet.h"
#include "key_manager.h"
#include <iostream>

Wallet::Wallet() = default;

void Wallet::generateKeyPair() {
    auto keyPair = KeyManager::generateKeyPair();
    privateKeyWIF = keyPair.privateKeyWIF;
    publicKeyHex = keyPair.publicKeyHex;
    testnetAddress = keyPair.testnetAddress;
}

void Wallet::showPublicAddress() const {
    std::cout << "Your Bitcoin Testnet address: " << testnetAddress << std::endl;
}