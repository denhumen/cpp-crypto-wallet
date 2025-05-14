//
// Created by denhumen on 5/14/25.
//

#include "WalletManager.hpp"
#include "EthereumWallet.hpp"
#include "SolanaWallet.hpp"
#include <TrustWalletCore/TWHDWallet.h>
#include <TrustWalletCore/TWString.h>
#include <iostream>
#include <qlogging.h>

WalletManager::WalletManager() : currentCoin(TWCoinTypeEthereum), wallet(nullptr) {}

WalletManager::~WalletManager() {
    logout();
}

WalletManager& WalletManager::instance() {
    static WalletManager instance;
    return instance;
}

std::string WalletManager::createNewWallet() {
    logout();

    wallet = TWHDWalletCreate(128, TWStringCreateWithUTF8Bytes(""));
    mnemonic = TWStringUTF8Bytes(TWHDWalletMnemonic(wallet));
    return mnemonic;
}

bool WalletManager::loadWallet(const std::string& mnemonic) {
    if (mnemonic.empty()) {
        std::cerr << "Error: Mnemonic cannot be empty.\n";
        return false;
    }

    logout();

    this->mnemonic = mnemonic;
    auto secretMnemonic = TWStringCreateWithUTF8Bytes(mnemonic.c_str());
    wallet = TWHDWalletCreateWithMnemonic(secretMnemonic, TWStringCreateWithUTF8Bytes(""));
    TWStringDelete(secretMnemonic);

    return wallet != nullptr;
}

void WalletManager::logout() {
    mnemonic.clear();

    if (wallet) {
        TWHDWalletDelete(wallet);
        wallet = nullptr;
    }

    cryptoWallet.reset();
    currentCoin = TWCoinTypeEthereum;
}

std::string WalletManager::getCurrentMnemonic() const {
    return mnemonic;
}

bool WalletManager::isWalletLoaded() const {
    return wallet != nullptr;
}

void WalletManager::setCurrentCoin(TWCoinType coinType) {
    if (!wallet) {
        std::cerr << "Error: Wallet not loaded.\n";
        return;
    }

    currentCoin = coinType;

    if (coinType == TWCoinTypeEthereum) {
        cryptoWallet = std::make_unique<EthereumWallet>();
    } else if (coinType == TWCoinTypeSolana) {
        cryptoWallet = std::make_unique<SolanaWallet>();
    } else {
        std::cerr << "Error: Unsupported coin type.\n";
        cryptoWallet = nullptr;
    }
}

std::string WalletManager::getCurrentAddress() const {
    if (!wallet) {
        return "Error: No wallet loaded.";
    }

    TWString* address = TWHDWalletGetAddressForCoin(wallet, currentCoin);
    std::string addressStr = TWStringUTF8Bytes(address);
    TWStringDelete(address);

    return addressStr;
}

std::string WalletManager::checkBalance() const {
    if (!cryptoWallet) {
        return "Error: No cryptocurrency selected.";
    }

    std::string address = getCurrentAddress();
    if (address.empty()) {
        return "Error: Unable to retrieve address.";
    }

    return cryptoWallet->getBalanceAsString(address);
}

TWHDWallet* WalletManager::getHdWallet() const {
    return wallet;
}

std::string WalletManager::sendTransaction(const std::string& toAddress, double amount) const {
    if (!cryptoWallet || !wallet) {
        return "Error: no wallet loaded";
    }
    // delegate down into EthereumWallet or SolanaWallet
    return cryptoWallet->signTransaction(wallet, toAddress, amount);
}

double WalletManager::getCurrentBalanceDouble() const {
    if (!cryptoWallet || !wallet) return 0.0;
    const auto addr = getCurrentAddress();
    return cryptoWallet->getBalanceAsDouble(addr);
}


