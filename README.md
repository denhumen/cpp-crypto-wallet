# 🛠️ C++ Multi-Chain CLI Wallet

A modern command-line cryptocurrency wallet built with C++17 that supports Solana (testnet), Ethereum (testnet), and Bitcoin (mainnet).

## 🚀 Features

- ✅ Create and restore wallets using mnemonic phrases
- 🌐 Check balances (SOL, ETH, BTC)
- 🎁 Request airdrops on Solana testnet
- 💸 Send transactions (Solana fully supported, Ethereum WIP)

## 📦 Requirements

- Clang (C++17 support required)
- CMake ≥ 3.16
- Trust Wallet Core (built locally)
- OpenSSL
- cURL
- LibSodium
- nlohmann/json

## 🔧 Building

### 1. Clone and build Trust Wallet Core

```bash
git clone --recurse-submodules https://github.com/trustwallet/wallet-core ../wallet-core
cd ../wallet-core
./tools/install-deps
make -j
