#ifndef CPP_CRYPTO_WALLET_BASE58_HPP
#define CPP_CRYPTO_WALLET_BASE58_HPP

#include <string>
#include <vector>

class Base58 {
public:
    static std::string encode(const std::vector<unsigned char>& data);
    static std::vector<uint8_t> decode(const std::string& encoded);
};
#endif //CPP_CRYPTO_WALLET_BASE58_HPP
