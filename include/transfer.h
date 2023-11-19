/**
 * @file transfer.h
 * @author CaddiesNew
 * @brief 
 * @version 0.1
 * @date 2023-11-19
 * 
 * @copyright Copyright (c) 2023
 *  
 * 定义一系列OpenSSL与string之间的转换函数
 */
#ifndef TRANSFER_H
#define TRANSFER_H
#pragma once

#include <gmpxx.h>
#include <openssl/bn.h>
#include <openssl/ec.h>
#include <openssl/evp.h>
#include <openssl/obj_mac.h>
#include <time.h>

#include <chrono>
#include <iostream>
#include <string>
#include <vector>

std::string getTimeTamp();

std::string point2hex(const EC_GROUP *ec_group, const EC_POINT *point);

std::string bn2hex(const BIGNUM *bn);

std::string bn2dec(const BIGNUM *bn);

BIGNUM *string2bn(const std::string &str);

BIGNUM *dec2bn(const std::string &dec);

BIGNUM *hex2bn(const std::string &hex);

EC_POINT *hex2point(const EC_GROUP *ec_group, const std::string &hex);

/**
 * @brief
 *  计算全部用 16 进制
 * @param a : hex
 * @param b : hex
 * @return std::string
 */
std::string quickPow(std::string &a_hex, std::string &b_hex,
                     const std::string &N_hex);

#endif  // TRANSFER_H