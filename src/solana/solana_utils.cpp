#include "solana/solana_utils.hpp"
#include <vector>
#include <string>
#include "base58.hpp"

std::string UtilsSolana::base58Encode(const unsigned char *input, size_t length) {
    std::vector<unsigned char> data(input, input + length);
    return Base58::encode(data);
}
