#ifndef CPP_CRYPTO_WALLET_SOLANA_NETWORK_HPP
#define CPP_CRYPTO_WALLET_SOLANA_NETWORK_HPP

#include <string>

class NetworkSolana {
public:
    static std::string getBalance(const std::string& base58Address);
    static std::string requestAirdrop(const std::string& address, int64_t lamports);
};

#endif //CPP_CRYPTO_WALLET_SOLANA_NETWORK_HPP
