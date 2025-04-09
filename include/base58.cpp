#include "base58.hpp"

static const std::string BASE64_ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZ" "abcdefghijklmnopqrstuvwxyz" "0123456789+/";


std::string Base64::encode(const std::vector<unsigned char>& data) {
    std::string result; int val = 0; int bits = -6; const unsigned int b63 = 0x3F;

    for (unsigned char c : data) {
        val = (val << 8) + c;
        bits += 8;

        while (bits >= 0) {
            result.push_back(BASE64_ALPHABET[(val >> bits) & b63]);
            bits -= 6;
        }
    }

    if (bits > -6)
        result.push_back(BASE64_ALPHABET[((val << 8) >> (bits + 8)) & b63]);

    while (result.size() % 4)
        result.push_back('=');

    return result;

}

std::vector<unsigned char> Base64::decode(const std::string& encoded) {
    std::vector<int> T(256, -1); for (int i = 0; i < 64; i++) T[BASE64_ALPHABET[i]] = i;
    std::vector<unsigned char> output;
    int val = 0, bits = -8;

    for (unsigned char c : encoded) {
        if (T[c] == -1) break;
        val = (val << 6) + T[c];
        bits += 6;
        if (bits >= 0) {
            output.push_back((val >> bits) & 0xFF);
            bits -= 8;
        }
    }

    return output;

}


