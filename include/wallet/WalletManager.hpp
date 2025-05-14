//
// Created by denhumen on 5/14/25.
//

#ifndef WALLETMANAGER_HPP
#define WALLETMANAGER_HPP

#include "CryptoWallet.hpp"
#include <string>
#include <memory>
#include <TrustWalletCore/TWCoinType.h>
#include <TrustWalletCore/TWHDWallet.h>

class WalletManager {
public:
    static WalletManager& instance();
    std::string createNewWallet();
    bool loadWallet(const std::string& mnemonic);
    std::string getCurrentMnemonic() const;
    bool isWalletLoaded() const;
    std::string getCurrentAddress() const;
    std::string checkBalance() const;
    void setCurrentCoin(TWCoinType coinType);
    void logout();
    TWHDWallet* getHdWallet() const;
    std::string sendTransaction(const std::string& toAddress, double amount) const;
    double getCurrentBalanceDouble() const;

private:
    WalletManager();
    ~WalletManager();
    WalletManager(const WalletManager&) = delete;
    WalletManager& operator=(const WalletManager&) = delete;

    std::string mnemonic;
    std::unique_ptr<CryptoWallet> cryptoWallet;
    TWCoinType currentCoin;
    TWHDWallet* wallet;
};

#endif //WALLETMANAGER_HPP
