#include "solana/solana_key_manager.hpp"
#include <sodium.h>
#include "solana/solana_utils.hpp"

SolanaKeyPair SolanaKeyManager::generateKeyPair() {
    SolanaKeyPair kp;

    unsigned char publicKey[crypto_sign_PUBLICKEYBYTES];
    unsigned char privateKey[crypto_sign_SECRETKEYBYTES];

    crypto_sign_keypair(publicKey, privateKey);

    kp.privateKeyBase58 = UtilsSolana::base58Encode(privateKey, crypto_sign_SECRETKEYBYTES);
    kp.publicKeyBase58 = UtilsSolana::base58Encode(publicKey, crypto_sign_PUBLICKEYBYTES);

    return kp;
}
