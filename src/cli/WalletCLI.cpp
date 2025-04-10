//
// Created by denhumen on 4/10/25.
//

#include "cli/WalletCLI.hpp"
#include "solana/SolanaWallet.hpp"
#include <iostream>
#include <memory>
#include <limits>

void WalletCLI::run() {
    while (true) {
        std::cout << "\n=== Crypto Wallet ===\n"
                  << "1. Register New Wallet\n"
                  << "2. Login to Existing Wallet\n"
                  << "3. Exit\n"
                  << "Choose option: ";

        int choice;
        std::cin >> choice;

        if (std::cin.fail()) {
            std::cin.clear(); // reset error flag
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // discard bad input
            continue;
        }

        auto wallet = std::make_unique<SolanaWallet>();

        if (choice == 1) {
            wallet->generateKeyPair();
            std::cout << "\nWallet successfully created!\n";
            wallet->showPublicAddress();
            wallet->showPrivateAddress();
        } else if (choice == 2) {
            std::string publicAddress, privateAddress;
            std::cout << "Enter your public key: ";
            std::cin >> publicAddress;
            std::cout << "Enter your private key: ";
            std::cin >> privateAddress;

            wallet->setPublicKey(publicAddress);
            wallet->setPrivateKey(privateAddress);
            // Simulate login by restoring state
            // (in full version you’d validate it from a file/db)

            std::cout << std::endl << "Logged in to wallet " << publicAddress << "\n";

            int action;
            do {
                std::cout << "\n1. Show Public Address\n"
                          << "2. Check Balance\n"
                          << "3. Airdrop SOL\n"
                          << "4. Make a transaction\n"
                          << "5. Exit to Main Menu\n"
                          << "Choose action: ";
                std::cin >> action;

                std::cout << std::endl;

                if (action == 1) {
                    wallet->showPublicAddress();
                } else if (action == 2) {
                    wallet->checkBalance();
                } else if (action == 3) {
                    int amount;
                    std::cout << "Enter amount: ";
                    std::cin >> amount;
                    wallet.get()->airdropAndCheckBalance(amount);
                } else if (action == 4) {
                    std::string publicKeyToSend;
                    std::cout << "Enter public key: ";
                    std::cin >> publicKeyToSend;
                    int amountToSend;
                    std::cout << "Enter amount: ";
                    std::cin >> amountToSend;

                    wallet.get()->sendSomeToOtherWallet(publicKeyToSend, amountToSend);
                }

            } while (action != 5);
        } else if (choice == 3) {
            std::cout << "Exiting. Goodbye!\n";
            break;
        }
    }
}
