#pragma once

#include "gmpxx.h"

#define _KEY_LEN 256

namespace utils {
/**
 * @brief Extended Euclidean algorithm
 *
 * @return mpz_class : gcd(a, b)
 */
mpz_class ex_gcd(mpz_class a, mpz_class b, mpz_class &x, mpz_class &y);

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
mpz_class quick_mul(mpz_class a, mpz_class b, mpz_class mod);
/**
 * @brief (base ^ power) % mod with gmp
 *
 * @return mpz_class
 */
mpz_class quick_pow(mpz_class base, mpz_class power, mpz_class mod);

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

/**
 * @brief compute the md5 checksum of a file
 * 
 * @param path 
 * @return std::string 
 */
std::string compute_file_md5(const std::string& path);

/**
 * @brief covert md5 checksum to entityId (PID as proposed in the drafts)
 *  for decode: mpz_class result;
    mpz_set_str(result.get_mpz_t(), pid_str.c_str(), 16);
 * @param md5str 
 * @return std::string 
 */
std::string md5_to_entityId(const std::string& md5str); 


void md5_to_id_test();
void test();
} // namespace utils
