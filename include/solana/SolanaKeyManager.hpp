//
// Created by denhumen on 4/9/25.
//

#ifndef SOLANAKEYMANAGER_HPP
#define SOLANAKEYMANAGER_HPP

#pragma once
#include <string>
#include "interfaces/IKeyManager.hpp"

class SolanaKeyManager final : public IKeyManager {
public:
    KeyPair generateKeyPair() override;
    ~SolanaKeyManager() override;
};


#endif //SOLANAKEYMANAGER_HPP
