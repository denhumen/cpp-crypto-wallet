#ifndef CPP_CRYPTO_WALLET_SOLANA_UTILS_HPP
#define CPP_CRYPTO_WALLET_SOLANA_UTILS_HPP

#include <string>
#include <vector>

class UtilsSolana {
public:
    static std::string base58Encode(const unsigned char *input, size_t length);

    static std::vector<unsigned char> base58Decode(const std::string &encoded);
};

#endif //CPP_CRYPTO_WALLET_SOLANA_UTILS_HPP
