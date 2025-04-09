//
// Created by denhumen on 4/9/25.
//

#ifndef KEY_MANAGER_H
#define KEY_MANAGER_H

#pragma once
#include <string>

struct KeyPair {
    std::string privateKeyWIF;
    std::string publicKeyHex;
    std::string testnetAddress;
};

class KeyManager {
public:
    static KeyPair generateKeyPair();
};

#endif //KEY_MANAGER_H
