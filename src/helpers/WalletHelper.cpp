//
// Created by denhumen on 5/14/25.
//

#include "WalletHelper.hpp"
#include <TrustWalletCore/TWHDWallet.h>
#include <TrustWalletCore/TWString.h>
#include <iostream>

std::string WalletHelper::createNewWallet() {
    TWHDWallet* wallet = TWHDWalletCreate(128, TWStringCreateWithUTF8Bytes(""));
    std::string mnemonic = TWStringUTF8Bytes(TWHDWalletMnemonic(wallet));
    TWHDWalletDelete(wallet);

    return mnemonic;
}

TWHDWallet* WalletHelper::getWalletFromMnemonic(const std::string& mnemonic) {
    if (mnemonic.empty()) {
        std::cerr << "Error: Mnemonic cannot be empty.\n";
        return nullptr;
    }

    auto secretMnemonic = TWStringCreateWithUTF8Bytes(mnemonic.c_str());
    TWHDWallet* wallet = TWHDWalletCreateWithMnemonic(secretMnemonic, TWStringCreateWithUTF8Bytes(""));
    TWStringDelete(secretMnemonic);

    return wallet;
}