//
// Created by denhumen on 5/14/25.
//

#ifndef ETHEREUMWALLET_HPP
#define ETHEREUMWALLET_HPP

#include <cstdint>
#include "CryptoWallet.hpp"

class EthereumWallet : public CryptoWallet {
public:
    double getBalanceAsDouble(const std::string& address) const override;
    std::string getBalanceAsString(const std::string& address) const override;
    std::string signTransaction(TWHDWallet* wallet, const std::string& toAddress, double amountEth) const override;
};

#endif //ETHEREUMWALLET_HPP
