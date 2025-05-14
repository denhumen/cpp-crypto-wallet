//
// Created by denhumen on 5/14/25.
//

#ifndef SOLANAWALLET_HPP
#define SOLANAWALLET_HPP

#include "CryptoWallet.hpp"

class SolanaWallet : public CryptoWallet {
public:
    double getBalanceAsDouble(const std::string& address) const override;
    std::string getBalanceAsString(const std::string& address) const override;
    std::string signTransaction(TWHDWallet* wallet, const std::string& toAddress, double amountEth) const override;
};

#endif //SOLANAWALLET_HPP
