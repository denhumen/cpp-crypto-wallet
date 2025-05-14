# cpp-crypto-wallet [Qt version]
A lightweight and secure Crypto Testnet wallet built in modern C++ with Qt6 framework for GUI

Branches:
- master: Qt version of cpp-crypto-wallet
- TrustWallet_CLI: CLI version of cpp-crypto-wallet

### 1) System dependencies (Debian/Ubuntu)

```bash
sudo apt update
sudo apt install -y \
  clang \
  cmake \
  ninja-build \
  pkg-config \
  libssl-dev \
  libcurl4-openssl-dev \
  libprotobuf-dev \
  protobuf-compiler \
  nlohmann-json3-dev \
  libsodium-dev \
  libsecp256k1-dev \
  qt6-base-dev \
  qt6-base-dev-tools \
  curl
```

### 2) Download and build Trust Wallet Core (Debian/Ubuntu)

```bash
sudo apt update
```

Then navigate to folder in your system where you want to install library (in can be Downloads for example)

Then follow next instructions:

```bash
git clone https://github.com/trustwallet/wallet-core.git
```

```bash
cd wallet-core
```

```bash
./bootstrap.sh native
```

```bash
CC=clang CXX=clang++ cmake -H. -Bbuild -DCMAKE_BUILD_TYPE=Debug
```

```bash
make -Cbuild -j12
```

For now the  

If you got any errors, see the official docs to solve them: https://developer.trustwallet.com/developer/wallet-core/developing-the-library/building

### 3) Properly setup CMakeLists.txt  (Debian/Ubuntu)

Chenge this line in cpp-crypto-wallet CMakeLists.txt:

```
set(WALLET_CORE your-proper-pass)
```

Here must be the direct pass to the library you just installed

#### After this your library files must be set up, check them in wallet-core/build folder. If everything is okay - you are ready to build cpp-crypto-wallet

