//
// Created by denhumen on 5/13/25.
//

#ifndef SOLANAHELPER_HPP
#define SOLANAHELPER_HPP

#include <string>

class SolanaHelper {
public:
    static std::string getBalance(const std::string& address);
    static std::string requestAirdrop(const std::string& address, int64_t lamports);
    static std::string transfer(
            const std::string& mnemonic,
            const std::string& toAddress,
            int64_t lamports
    );
};

#endif //SOLANAHELPER_HPP
