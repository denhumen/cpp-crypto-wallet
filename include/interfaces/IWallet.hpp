//
// Created by denhumen on 4/9/25.
//

#ifndef IWALLET_HPP
#define IWALLET_HPP

#pragma once
#include <string>

class IWallet {
public:
    virtual ~IWallet() = default;
    virtual void generateKeyPair() = 0;
    virtual void showPublicAddress() const = 0;
    virtual void showPrivateAddress() const = 0;
    virtual void checkBalance() const = 0;

    virtual void airdropAndCheckBalance(long amount) const = 0;
};

#endif //IWALLET_HPP
