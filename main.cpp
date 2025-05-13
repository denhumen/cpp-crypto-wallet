// SPDX-License-Identifier: Apache-2.0
//
// Copyright © 2017 Trust Wallet.

#include <TrustWalletCore/TWHDWallet.h>
#include <TrustWalletCore/TWCoinType.h>
#include <TrustWalletCore/TWPrivateKey.h>
#include <TrustWalletCore/TWString.h>
#include <curl/curl.h>
#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include "JsonRpcHelper.hpp"
#include "SolanaHelper.hpp"

using namespace std;

const string WALLET_FILE = "wallets.dat";

// Store wallet in memory (in a real application, this should be securely stored)
map<string, string> registeredWallets;

// Function to load wallets from file
void loadWallets() {
    ifstream file(WALLET_FILE);
    if (file.is_open()) {
        string username, mnemonic;
        while (getline(file, username) && getline(file, mnemonic)) {
            registeredWallets[username] = mnemonic;
        }
        file.close();
    }
}

// Function to save wallets to file
void saveWallets() {
    ofstream file(WALLET_FILE);
    if (file.is_open()) {
        for (const auto& [username, mnemonic] : registeredWallets) {
            file << username << endl;
            file << mnemonic << endl;
        }
        file.close();
    }
}

// Function to create a new wallet
void registerWallet() {
    cout << "\n🔐 Register a New Wallet\n";
    cout << "Enter a username: ";
    string username;
    cin >> username;

    // Create a new wallet
    TWHDWallet* wallet = TWHDWalletCreate(128, TWStringCreateWithUTF8Bytes(""));
    string mnemonic = TWStringUTF8Bytes(TWHDWalletMnemonic(wallet));

    // Store mnemonic in memory and file (secure this in real apps)
    registeredWallets[username] = mnemonic;
    saveWallets();

    cout << "\n✅ Registration Successful!\n";
    cout << "🔑 Your Recovery Mnemonic (WRITE IT DOWN SAFELY):\n";
    cout << mnemonic << endl;
    TWHDWalletDelete(wallet);
}

// Function to log in using an existing wallet
TWHDWallet* loginWallet() {
    cout << "\n🔐 Login to Your Wallet\n";
    cout << "Enter your username: ";
    string username;
    cin >> username;

    // Check if the user is registered
    if (registeredWallets.find(username) == registeredWallets.end()) {
        cout << "❌ Username not found. Please register first.\n";
        return nullptr;
    }

    string mnemonic = registeredWallets[username];
    auto secretMnemonic = TWStringCreateWithUTF8Bytes(mnemonic.c_str());
    TWHDWallet* wallet = TWHDWalletCreateWithMnemonic(secretMnemonic, TWStringCreateWithUTF8Bytes(""));
    TWStringDelete(secretMnemonic);

    cout << "\n✅ Login Successful!\n";

    cout << "\n🌐 Your Wallet Addresses:\n";
    cout << "🔸 Bitcoin: " << TWStringUTF8Bytes(TWHDWalletGetAddressForCoin(wallet, TWCoinTypeBitcoin)) << endl;
    cout << "🔸 Ethereum: " << TWStringUTF8Bytes(TWHDWalletGetAddressForCoin(wallet, TWCoinTypeEthereum)) << endl;
    cout << "🔸 Solana: " << TWStringUTF8Bytes(TWHDWalletGetAddressForCoin(wallet, TWCoinTypeSolana)) << endl;

    return wallet;
}

// Function to airdrop SOL (Solana testnet)
// void airdropSolana(TWHDWallet* wallet) {
//     const string solanaRpcUrl = "https://api.testnet.solana.com";
//     string address = TWStringUTF8Bytes(TWHDWalletGetAddressForCoin(wallet, TWCoinTypeSolana));
//
//     // Airdrop request
//     string command = "curl -X POST " + solanaRpcUrl + R"(/ -H "Content-Type: application/json" -d '{"jsonrpc":"2.0","id":1,"method":"requestAirdrop","params":[")" + address + R"(", 1000000000]}')";
//     cout << "🚀 Airdropping 1 SOL to address: " << address << "\n";
//     system(command.c_str());
// }
void airdropSolana(TWHDWallet* wallet) {
    string address = TWStringUTF8Bytes(TWHDWalletGetAddressForCoin(wallet, TWCoinTypeSolana));
    int64_t lamports = 1000000000;  // 1 SOL = 1,000,000,000 lamports

    cout << "🚀 Airdropping 1 SOL to address: " << address << "\n";

    // Use SolanaHelper to request airdrop
    string response = SolanaHelper::requestAirdrop(address, lamports);
    cout << "Airdrop Response: " << response << "\n";
}

// Function to check Solana balance (testnet)
// void checkSolanaBalance(TWHDWallet* wallet) {
//     const string solanaRpcUrl = "https://api.testnet.solana.com";
//     string address = TWStringUTF8Bytes(TWHDWalletGetAddressForCoin(wallet, TWCoinTypeSolana));
//
//     // Balance request
//     string command = "curl -s -X POST " + solanaRpcUrl + R"(/ -H "Content-Type: application/json" -d '{"jsonrpc":"2.0","id":1,"method":"getBalance","params":[")" + address + R"("]}')";
//     cout << "\n🔍 Checking Solana balance for address: " << address << "\n";
//     system(command.c_str());
// }

// Function to check Solana balance (testnet)
void checkSolanaBalance(TWHDWallet* wallet) {
    string address = TWStringUTF8Bytes(TWHDWalletGetAddressForCoin(wallet, TWCoinTypeSolana));

    cout << "\n🔍 Checking Solana balance for address: " << address << "\n";

    // Use SolanaHelper to get the balance
    string response = SolanaHelper::getBalance(address);
    cout << "Balance Response: " << response << "\n";
}

// Main application logic
int main() {
    // Load existing wallets
    loadWallets();
    cout << "🔐 Welcome to Your Crypto Wallet\n";
    TWHDWallet* loggedInWallet = nullptr;

    while (true) {
        cout << "\n1️⃣  Register New Wallet\n";
        cout << "2️⃣  Login to Existing Wallet\n";
        cout << "3️⃣  Login with Mnemonic\n";
        cout << "4️⃣  Airdrop SOL (Testnet)\n";
        cout << "5️⃣  Check Balance (Solana)\n";
        cout << "6️⃣  Exit\n";
        cout << "Choose an option (1, 2, 3, 4, 5, 6): ";

        int choice;
        cin >> choice;

        if (choice == 1) {
            registerWallet();
        } else if (choice == 2) {
            loggedInWallet = loginWallet();
        } else if (choice == 3) {
            cout << "\n🔑 Enter your mnemonic phrase: ";
            string mnemonic;
            cin.ignore();
            getline(cin, mnemonic);
            auto secretMnemonic = TWStringCreateWithUTF8Bytes(mnemonic.c_str());
            loggedInWallet = TWHDWalletCreateWithMnemonic(secretMnemonic, TWStringCreateWithUTF8Bytes(""));
            TWStringDelete(secretMnemonic);

            cout << "\n✅ Login Successful!\n";

            // Display wallet addresses
            cout << "\n🌐 Your Wallet Addresses:\n";
            cout << "🔸 Bitcoin: " << TWStringUTF8Bytes(TWHDWalletGetAddressForCoin(loggedInWallet, TWCoinTypeBitcoin)) << endl;
            cout << "🔸 Ethereum: " << TWStringUTF8Bytes(TWHDWalletGetAddressForCoin(loggedInWallet, TWCoinTypeEthereum)) << endl;
            cout << "🔸 Solana: " << TWStringUTF8Bytes(TWHDWalletGetAddressForCoin(loggedInWallet, TWCoinTypeSolana)) << endl;

        } else if (choice == 4) {
            if (!loggedInWallet) {
                cout << "❌ Please log in first.\n";
            } else {
                airdropSolana(loggedInWallet);
            }
        } else if (choice == 5) {
            if (!loggedInWallet) {
                cout << "❌ Please log in first.\n";
            } else {
                checkSolanaBalance(loggedInWallet);
            }
        } else if (choice == 6) {
            cout << "🚪 Exiting. Goodbye!\n";
            break;
        } else {
            cout << "❌ Invalid option. Try again.\n";
        }
    }

    // Save wallets on exit
    saveWallets();
    return 0;
}
