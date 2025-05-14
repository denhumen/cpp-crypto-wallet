//
// Created by denhumen on 5/14/25.
//

#ifndef WALLETHELPER_HPP
#define WALLETHELPER_HPP

#include <string>
#include <TrustWalletCore/TWHDWallet.h>

class WalletHelper {
public:
    static std::string createNewWallet();
    static TWHDWallet* getWalletFromMnemonic(const std::string& mnemonic);
};

#endif //WALLETHELPER_HPP
