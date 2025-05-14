#ifndef CPP_CRYPTO_WALLET_BITCOINHELPER_HPP
#define CPP_CRYPTO_WALLET_BITCOINHELPER_HPP

#include <string>
#include <TrustWalletCore/TWCoinType.h>


class BitCoinHelper{
public:
    static std::string getBalance(const std::string& address);
    static std::string requestAirdrop(const std::string& address, int64_t lamports);
};

#endif //CPP_CRYPTO_WALLET_BITCOINHELPER_HPP
