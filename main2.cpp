#include <sodium/core.h>
#include <iostream>
#include <thread>
#include "solana/solana_wallet.hpp"

int main(){

    if (sodium_init() < 0) {
        std::cerr << "Failed to initialize libsodium." << std::endl;
        return 1;
    }

    SolanaWallet solana;
    solana.generateKeyPair();
    solana.showPublicAddress();
    solana.airdropAndCheckBalance(2);

    solana.sendSomeToOtherWallet("3UTfRqevbZuZSWg1CSFjs1WQByYKEVarVQBx8LLWVSnD", 1);


    for(int i = 0; i < 5; i++){
        std::this_thread::sleep_for(std::chrono::seconds(3));

        solana.checkBalance();
    }

    return 0;
}