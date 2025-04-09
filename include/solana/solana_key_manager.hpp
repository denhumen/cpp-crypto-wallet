
#ifndef CPP_CRYPTO_WALLET_SOLANA_KEY_MANAGER_HPP
#define CPP_CRYPTO_WALLET_SOLANA_KEY_MANAGER_HPP
#include <string>

struct SolanaKeyPair {
    std::string privateKeyBase58;
    std::string publicKeyBase58;
};

class SolanaKeyManager {
public:
    static SolanaKeyPair generateKeyPair();
};
#endif //CPP_CRYPTO_WALLET_SOLANA_KEY_MANAGER_HPP
