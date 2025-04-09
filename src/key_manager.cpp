//
// Created by denhumen on 4/9/25.
//

#include "key_manager.h"
#include "utils.h"
#include <openssl/ec.h>
#include <openssl/obj_mac.h>
#include <openssl/sha.h>
#include <openssl/ripemd.h>
#include <vector>
#include <sstream>
#include <iomanip>

KeyPair KeyManager::generateKeyPair() {
    KeyPair kp;

    EC_KEY* ecKey = EC_KEY_new_by_curve_name(NID_secp256k1);
    EC_KEY_generate_key(ecKey);

    const BIGNUM* priv = EC_KEY_get0_private_key(ecKey);
    char* privHex = BN_bn2hex(priv);
    kp.privateKeyWIF = Utils::privateKeyToWIF(privHex, true); // true = testnet

    const EC_POINT* pub = EC_KEY_get0_public_key(ecKey);
    EC_GROUP* group = EC_GROUP_new_by_curve_name(NID_secp256k1);
    char* pubHex = EC_POINT_point2hex(group, pub, POINT_CONVERSION_UNCOMPRESSED, NULL);
    kp.publicKeyHex = pubHex;

    kp.testnetAddress = Utils::publicKeyToAddress(pubHex, true);

    EC_KEY_free(ecKey);
    EC_GROUP_free(group);
    OPENSSL_free(privHex);
    OPENSSL_free(pubHex);
    return kp;
}