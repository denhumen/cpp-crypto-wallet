//
// Created by denhumen on 4/9/25.
//

#ifndef WALLET_H
#define WALLET_H

#pragma once
#include <string>

class Wallet {
public:
    Wallet();
    void generateKeyPair();
    void showPublicAddress() const;
private:
    std::string privateKeyWIF;
    std::string publicKeyHex;
    std::string testnetAddress;
};

#endif //WALLET_H
