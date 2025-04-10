//
// Created by denhumen on 4/9/25.
//

#ifndef INETWORK_HPP
#define INETWORK_HPP

#pragma once
#include <string>

class INetwork {
public:
    virtual ~INetwork() = default;
    virtual std::string getBalance(const std::string& address) const = 0;
};

#endif //INETWORK_HPP
