#include <sodium/core.h>
#include <iostream>
#include "solana/solana_wallet.hpp"

int main(){

    if (sodium_init() < 0) {
        std::cerr << "Failed to initialize libsodium." << std::endl;
        return 1;
    }

    SolanaWallet solana;
    solana.generateKeyPair();
    solana.showPublicAddress();
    solana.airdropAndCheckBalance(1);

    solana.loopCheckBalance();

    return 0;
}