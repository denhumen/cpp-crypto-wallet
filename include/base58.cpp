#include "base58.hpp"

static const std::string BASE58_ALPHABET = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";

std::string Base58::encode(const std::vector<unsigned char>& data) {
    if (data.empty()) return "";

    std::vector<unsigned char> input(data.begin(), data.end());
    std::vector<unsigned char> digits((input.size() * 138 / 100) + 1);
    size_t digitlen = 1;

    for (unsigned char byte : input) {
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
    for (unsigned char byte : input) {
        if (byte == 0) result += '1';
        else break;
    }

    for (ssize_t i = digitlen - 1; i >= 0; --i)
        result += BASE58_ALPHABET[digits[i]];

    return result;
}

std::vector<unsigned char> Base58::decode(const std::string& encoded) {
    std::vector<int> map(256, -1);
    for (int i = 0; i < 58; i++) map[BASE58_ALPHABET[i]] = i;

    std::vector<unsigned char> bytes((encoded.size() * 733 / 1000) + 1);
    size_t byteLen = 1;

    for (char c : encoded) {
        if (map[c] == -1) break;
        int carry = map[c];
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

    std::vector<unsigned char> result(leadingZeros, 0x00);
    for (ssize_t i = byteLen - 1; i >= 0; --i) {
        result.push_back(bytes[i]);
    }

    return result;
}
