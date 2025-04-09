#ifndef CPP_CRYPTO_WALLET_SOLANA_NETWORK_HPP
#define CPP_CRYPTO_WALLET_SOLANA_NETWORK_HPP

#include <string>

class NetworkSolana {
public:
    static std::string getBalance(const std::string& base58Address);
};

#endif //CPP_CRYPTO_WALLET_SOLANA_NETWORK_HPP
