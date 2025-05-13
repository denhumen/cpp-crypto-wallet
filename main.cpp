// SPDX-License-Identifier: Apache-2.0
//
// Copyright © 2017 Trust Wallet.

#include <TrustWalletCore/TWHDWallet.h>
#include <TrustWalletCore/TWCoinType.h>
#include <TrustWalletCore/TWPrivateKey.h>
#include <TrustWalletCore/TWString.h>

#include <iostream>
#include <string>
#include <map>

using namespace std;

// Store wallet in memory (in a real application, this should be securely stored)
map<string, string> registeredWallets; // key: username, value: mnemonic

// Function to create a new wallet
void registerWallet() {
    cout << "\n🔐 Register a New Wallet\n";
    cout << "Enter a username: ";
    string username;
    cin >> username;

    // Create a new wallet
    TWHDWallet* wallet = TWHDWalletCreate(128, TWStringCreateWithUTF8Bytes(""));
    string mnemonic = TWStringUTF8Bytes(TWHDWalletMnemonic(wallet));

    // Store mnemonic in memory (secure this in real apps)
    registeredWallets[username] = mnemonic;
    cout << "\n✅ Registration Successful!\n";
    cout << "🔑 Your Recovery Mnemonic (WRITE IT DOWN SAFELY):\n";
    cout << mnemonic << endl;
    TWHDWalletDelete(wallet);
}

// Function to log in using an existing wallet
void loginWallet() {
    cout << "\n🔐 Login to Your Wallet\n";
    cout << "Enter your username: ";
    string username;
    cin >> username;

    // Check if the user is registered
    if (registeredWallets.find(username) == registeredWallets.end()) {
        cout << "❌ Username not found. Please register first.\n";
        return;
    }

    string mnemonic = registeredWallets[username];
    auto secretMnemonic = TWStringCreateWithUTF8Bytes(mnemonic.c_str());
    TWHDWallet* wallet = TWHDWalletCreateWithMnemonic(secretMnemonic, TWStringCreateWithUTF8Bytes(""));
    TWStringDelete(secretMnemonic);

    cout << "\n✅ Login Successful!\n";
    cout << "🔑 Your Recovery Mnemonic:\n";
    cout << mnemonic << endl;

    // Display wallet addresses
    cout << "\n🌐 Your Wallet Addresses:\n";
    cout << "🔸 Bitcoin: " << TWStringUTF8Bytes(TWHDWalletGetAddressForCoin(wallet, TWCoinTypeBitcoin)) << endl;
    cout << "🔸 Ethereum: " << TWStringUTF8Bytes(TWHDWalletGetAddressForCoin(wallet, TWCoinTypeEthereum)) << endl;
    cout << "🔸 Solana: " << TWStringUTF8Bytes(TWHDWalletGetAddressForCoin(wallet, TWCoinTypeSolana)) << endl;

    TWHDWalletDelete(wallet);
}

// Main application logic
int main() {
    cout << "🔐 Welcome to Your Crypto Wallet\n";
    while (true) {
        cout << "\n1️⃣ Register New Wallet\n";
        cout << "2️⃣ Login to Existing Wallet\n";
        cout << "3️⃣ Exit\n";
        cout << "Choose an option (1, 2, 3): ";

        int choice;
        cin >> choice;

        if (choice == 1) {
            registerWallet();
        } else if (choice == 2) {
            loginWallet();
        } else if (choice == 3) {
            cout << "🚪 Exiting. Goodbye!\n";
            break;
        } else {
            cout << "❌ Invalid option. Try again.\n";
        }
    }

    return 0;
}
