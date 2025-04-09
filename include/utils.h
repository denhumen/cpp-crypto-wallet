//
// Created by denhumen on 4/9/25.
//

#ifndef UTILS_H
#define UTILS_H

#pragma once
#include <string>

class Utils {
public:
    static std::string privateKeyToWIF(const std::string& hex, bool testnet);
    static std::string publicKeyToAddress(const std::string& pubKeyHex, bool testnet);
    static std::string base58CheckEncode(const std::string& dataHex);
};

#endif //UTILS_H
