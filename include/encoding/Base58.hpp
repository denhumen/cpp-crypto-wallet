
#ifndef BASE58_HPP
#define BASE58_HPP

#include <string>
#include <vector>
#include <cstdint>
#include <stdexcept>

class Base58 {
public:
    static std::string encode(const std::vector<unsigned char>& data);
    static std::vector<uint8_t> decode(const std::string& encoded);
};

#endif //BASE58_HPP
