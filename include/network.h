//
// Created by denhumen on 4/9/25.
//

#ifndef NETWORK_H
#define NETWORK_H

#pragma once
#include <string>

class Network {
public:
    static std::string fetchAddressInfo(const std::string& address);
    static void printBalanceFromResponse(const std::string& jsonResponse);
};

#endif //NETWORK_H
