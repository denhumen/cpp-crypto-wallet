//
// Created by denhumen on 5/13/25.
//

#ifndef ETHEREUMHELPER_HPP
#define ETHEREUMHELPER_HPP

#include <string>

class EthereumHelper {
public:
    static std::string getBalance(const std::string& address);
    static std::string requestAirdrop(const std::string& address, int64_t lamports);
};

#endif //ETHEREUMHELPER_HPP
