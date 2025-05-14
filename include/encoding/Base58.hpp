//
// Created by denhumen on 4/9/25.
//

#ifndef BASE58_HPP
#define BASE58_HPP

#include <string>
#include <vector>

class Base58 {
public:
    static std::string encode(const std::vector<unsigned char>& data);
    static std::vector<unsigned char> decode(const std::string& encoded);
};

#endif //BASE58_HPP
