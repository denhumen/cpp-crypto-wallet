#ifndef CPP_CRYPTO_WALLET_SOLANA_WALLET_HPP
#define CPP_CRYPTO_WALLET_SOLANA_WALLET_HPP

#include <string>

class SolanaWallet {
public:
    void generateKeyPair();
    void showPublicAddress() const;
    void checkBalance() const;
private:
    std::string privateKey;
    std::string publicKey;
};

#endif //CPP_CRYPTO_WALLET_SOLANA_WALLET_HPP
