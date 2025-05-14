//
// Created by denhumen on 4/9/25.
//

#include <cstdint>
#include <stdexcept>

#include "encoding/Base58.hpp"

static const std::string BASE58_ALPHABET = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";

std::string Base58::encode(const std::vector<unsigned char>& data) {
    if (data.empty()) return "";

    std::vector<unsigned char> input(data.begin(), data.end());
    std::vector<unsigned char> digits((input.size() * 138 / 100) + 1);
    size_t digitlen = 1;

    for (const unsigned char byte : input) {
        int carry = byte;
        for (size_t j = 0; j < digitlen; ++j) {
            carry += digits[j] << 8;
            digits[j] = carry % 58;
            carry /= 58;
        }
        while (carry) {
            digits[digitlen++] = carry % 58;
            carry /= 58;
        }
    }

    std::string result;
    for (const unsigned char byte : input) {
        if (byte == 0) result += '1';
        else break;
    }

    for (ssize_t i = digitlen - 1; i >= 0; --i)
        result += BASE58_ALPHABET[digits[i]];

    return result;
}

std::vector<uint8_t> Base58::decode(const std::string& encoded) {
    std::vector<int> map(256, -1);
    for (int i = 0; i < 58; i++) {
        map[BASE58_ALPHABET[i]] = i;
    }

    std::vector<uint8_t> bytes((encoded.size() * 733 / 1000) + 1, 0);
    size_t byteLen = 1;

    for (size_t idx = 0; idx < encoded.size(); idx++) {
        char c = encoded[idx];
        int value = map[c];
        if (value == -1) {
            throw std::runtime_error("Invalid base58 encoding: InvalidCharacter { character: '" +
                                     std::string(1, c) + "', index: " + std::to_string(idx) + " }");
        }
        int carry = value;
        for (size_t j = 0; j < byteLen; ++j) {
            carry += bytes[j] * 58;
            bytes[j] = carry & 0xFF;
            carry >>= 8;
        }
        while (carry) {
            bytes[byteLen++] = carry & 0xFF;
            carry >>= 8;
        }
    }

    size_t leadingZeros = 0;
    for (char c : encoded) {
        if (c == '1') leadingZeros++;
        else break;
    }

    std::vector<uint8_t> result(leadingZeros, 0x00);
    for (ssize_t i = byteLen - 1; i >= 0; --i) {
        result.push_back(bytes[i]);
    }

    return result;
}
