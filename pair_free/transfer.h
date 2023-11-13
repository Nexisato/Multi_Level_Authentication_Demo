#ifndef TRANSFER_H
#pragma once

#include <openssl/ec.h>
#include <openssl/evp.h>
#include <time.h>
#include <chrono>
#include <iostream>
#include <string>



std::string getTimeTamp() {
    auto now = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
    auto now_c = now.time_since_epoch().count();
    return std::to_string(now_c);

}


std::string point2hex(const EC_GROUP *ec_group, const EC_POINT *point) {
    char *point_hex = EC_POINT_point2hex(ec_group, point,
                                         POINT_CONVERSION_COMPRESSED, nullptr);
    std::string point_hex_str(point_hex);
    OPENSSL_free(point_hex);
    return point_hex_str;
}

std::string bn2hex(const BIGNUM *bn) {
    char *bn_hex = BN_bn2hex(bn);
    std::string bn_hex_str(bn_hex);
    OPENSSL_free(bn_hex);
    return bn_hex_str;
}

std::string bn2dec(const BIGNUM *bn) {
    char *bn_hex = BN_bn2dec(bn);
    std::string bn_hex_str(bn_hex);
    OPENSSL_free(bn_hex);
    return bn_hex_str;
}

BIGNUM *string2bn(const std::string &str) {
    BIGNUM *bn = BN_new();
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hashlen;
    
    if (EVP_Digest(reinterpret_cast<const unsigned char *>(str.c_str()),
                   str.length(), hash, &hashlen, EVP_sha256(), NULL)) {
        BN_bin2bn(hash, hashlen, bn);
        return bn;
    } else {
        BN_free(bn);  
        return nullptr; 
    }
}

BIGNUM *hex2bn(const std::string &hex) {
    BIGNUM *bn = BN_new();
    BN_hex2bn(&bn, const_cast<char *>(hex.c_str()));
    return bn;
}

EC_POINT *hex2point(const EC_GROUP *ec_group, const std::string &hex) {
    EC_POINT *point = EC_POINT_new(ec_group);
    EC_POINT_hex2point(ec_group, const_cast<char *>(hex.c_str()), point,
                       nullptr);
    return point;
}

#endif  // TRANSFER_H