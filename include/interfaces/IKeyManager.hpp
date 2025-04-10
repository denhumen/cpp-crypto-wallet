//
// Created by denhumen on 4/9/25.
//

#ifndef IKEYMANAGER_HPP
#define IKEYMANAGER_HPP

#pragma once
#include <string>

struct KeyPair {
    std::string privateKey;
    std::string publicKey;
};

class IKeyManager {
public:
    virtual ~IKeyManager() = default;
    virtual KeyPair generateKeyPair() = 0;
};

#endif //IKEYMANAGER_HPP
