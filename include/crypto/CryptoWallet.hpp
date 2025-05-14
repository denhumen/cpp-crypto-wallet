//
// Created by denhumen on 5/14/25.
//

#ifndef CRYPTOWALLET_HPP
#define CRYPTOWALLET_HPP

#include <string>
#include <TrustWalletCore/TWHDWallet.h>

class CryptoWallet {
public:
    virtual ~CryptoWallet() = default;
    virtual std::string getBalanceAsString(const std::string& address) const = 0;
    virtual double getBalanceAsDouble(const std::string& address) const = 0;
    virtual std::string signTransaction(TWHDWallet* wallet, const std::string& toAddress, double amountEth) const = 0;
};
#endif //CRYPTOWALLET_HPP
