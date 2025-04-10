#ifndef CPP_CRYPTO_WALLET_SOLANA_WALLET_HPP
#define CPP_CRYPTO_WALLET_SOLANA_WALLET_HPP

#include <string>
#include "solana_network.hpp"

class SolanaWallet {
    NetworkSolana solanaNet;
public:
    SolanaWallet(){
        solanaNet = NetworkSolana();
    }
    void generateKeyPair();
    void showPublicAddress() const;
    void checkBalance();
    void airdropAndCheckBalance(long amount);
    void sendSomeToOtherWallet(std::string recipientPub58, long amount);
    void loopCheckBalance();
    std::string checkAccountInfo();
private:
    std::string privateKey;
    std::string publicKey;
    std::string secretKey;
};

#endif //CPP_CRYPTO_WALLET_SOLANA_WALLET_HPP
