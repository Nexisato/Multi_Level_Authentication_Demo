#pragma once

#include <gmpxx.h>
#include <chrono>
#include <random>

const int _KEY_LEN = 1024;

using chrono_time = std::chrono::_V2::system_clock::time_point;

namespace utils {
/**
 * @brief Calculate the Time Duration between 2 points
 *
 * @param t0 : start_time
 * @param t1 : end_time
 * @return double / (unit):milliseconds
 */
double count_time(chrono_time t0, chrono_time t1);
/**
 * @brief  convert std::string vector to bytes vector
 *
 * @param str
 * @return std::vector<uint8_t>
 */
std::vector<uint8_t> stringToBytes(const std::string& str);
/**
 * @brief convert bytes vector to string vector
 *
 * @param bytes
 * @return std::string
 */
std::string bytesToString(const std::vector<uint8_t>& bytes);

/**
 * @brief xor two byte vectors
 * 
 * @param data1 
 * @param data2 
 * @return std::vector<uint8_t> 
 */
std::vector<uint8_t> xorBytes(const std::vector<uint8_t>& data1,
                              const std::vector<uint8_t>& data2);

/**
 * @brief Extended Euclidean algorithm
 *
 * @return mpz_class : gcd(a, b)
 */
mpz_class ex_gcd(mpz_class a, mpz_class b, mpz_class& x, mpz_class& y);

/**
 * @brief a * x = 1 (mod {mod})
 * @return mpz_class : x
 */
mpz_class mod_reverse(mpz_class a, mpz_class mod);

/**
 * @brief (a * b) % mod with gmp
 *
 * @return mpz_class
 */
// mpz_class quick_mul(mpz_class a, mpz_class b, mpz_class mod);
// /**
//  * @brief (base ^ power) % mod with gmp
//  *
//  * @return mpz_class
//  */
// mpz_class quick_pow(mpz_class base, mpz_class power, mpz_class mod);

/**
 * @brief judge whether num is prime with Miller-Rabin algorithm
 *:
 * @return true/false
 */
bool is_prime_miller_rabin(mpz_class num);

/**
 * @brief return a random big number with bits
 *
 * @param bits
 * @return mpz_class
 */
mpz_class rand_big_num(int bits);
mpz_class rand_prime(int bits);
mpz_class rand_safe_prime(int bits);
bool is_safe_prime(mpz_class num);


void test();
}  // namespace utils
