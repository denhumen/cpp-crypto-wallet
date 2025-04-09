#include "solana/solana_wallet.hpp"

int main(){
    SolanaWallet solana;
    solana.generateKeyPair();
    solana.showPublicAddress();
    solana.checkBalance();
}