#ifndef CPP_CRYPTO_WALLET_SOLANA_UTILS_HPP
#define CPP_CRYPTO_WALLET_SOLANA_UTILS_HPP

#include <string>

class UtilsSolana {
public:
    static std::string base58Encode(const unsigned char* input, size_t length);
};

#endif //CPP_CRYPTO_WALLET_SOLANA_UTILS_HPP
