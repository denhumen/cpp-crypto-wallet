
#ifndef CPP_CRYPTO_WALLET_SOLANA_KEY_MANAGER_HPP
#define CPP_CRYPTO_WALLET_SOLANA_KEY_MANAGER_HPP
#include <string>
#include <vector>

struct SolanaKeyPair {
    std::string secretKeyBase58;
    std::string publicKeyBase58;
    std::string privateKeyBase64;
};

class SolanaKeyManager {
public:
    static SolanaKeyPair generateKeyPair();
    static std::string base64Encode(const std::vector<unsigned char>& data);
    static std::vector<unsigned char> base64Decode(const std::string& encoded);
};
#endif //CPP_CRYPTO_WALLET_SOLANA_KEY_MANAGER_HPP
