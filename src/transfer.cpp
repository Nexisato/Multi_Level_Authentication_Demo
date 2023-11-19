/**
 * @file transfer.cpp
 * @author CaddiesNew
 * @brief 
 * @version 0.1
 * @date 2023-11-19
 * 
 * @copyright Copyright (c) 2023
 *  Implement the functions defined in transfer.h
 */
#include "../include/transfer.h"

std::string getTimeTamp() {
    auto now = std::chrono::time_point_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now());
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

BIGNUM *dec2bn(const std::string &dec) {
    BIGNUM *bn = BN_new();
    BN_dec2bn(&bn, const_cast<char *>(dec.c_str()));
    return bn;
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

/**
 * @brief
 *  计算全部给我用 16 进制
 * @param a : hex
 * @param b : hex
 * @return std::string
 */
std::string quickPow(std::string &a_hex, std::string &b_hex,
                     const std::string &N_hex) {
    // convert from hex
    mpz_class a(a_hex, 16);
    mpz_class b(b_hex, 16);
    mpz_class N(N_hex, 16);
    mpz_class c;
    std::cout << "a: " << a.get_str(16) << std::endl;
    std::cout << "b: " << b.get_str(16) << std::endl;
    std::cout << "N: " << N.get_str(16) << std::endl;
    mpz_powm(c.get_mpz_t(), a.get_mpz_t(), b.get_mpz_t(), N.get_mpz_t());
    // convert to hex
    return c.get_str(16);
}
